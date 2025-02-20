import asyncio
import os
import logging

import aiocoap
import httpx
from aiocoap import resource, Code
import re

# Only allow for WARNING logging from automatic loggers
logging.getLogger("httpx").setLevel(logging.WARNING)
logging.getLogger("urllib3").setLevel(logging.WARNING)
logging.getLogger("urllib3.connectionpool").setLevel(logging.WARNING)

# Configure logging
LOG_FILE = os.path.join(os.path.dirname(__file__), "coap_server.log")
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler(LOG_FILE, mode="a"),
        logging.StreamHandler()
    ]
)

# Get IPv6 address from systemd environment
coap_server_ip = os.getenv("COAP_SERVER_IP", "::1")  # Default to localhost (::1) if unset
logging.info(f"Using CoAP server IP: {coap_server_ip}")


class CoAPResource(resource.Resource):
    """CoAP Resource to handle POST requests"""
    async def render_post(self, request):
        try:
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            telegram_api_url = data.get("url", "").strip()
            telegram_bot_token = data.get("token", "").strip()
            chat_ids = data.get("chat_ids", "").strip()
            text = data.get("text", "").strip()
            text = text.replace("\x00", "").replace("\n", "")

            if not telegram_api_url or not telegram_bot_token or not chat_ids or not text:
                logging.error("Missing required fields in request")
                logging.error(f"url='{telegram_api_url}', bot_token=[HIDDEN], chat_ids={chat_ids}, text='{text}'")
                return aiocoap.Message(code=Code.BAD_REQUEST, payload=b"Missing required fields")


            logging.info(f"Received message request: url='{telegram_api_url}', bot_token=[HIDDEN], chat_ids={chat_ids}, text='{text}'")
            telegram_api_url = f"{telegram_api_url}{telegram_bot_token}"
            chat_ids_list = chat_ids.split(",")

            async with httpx.AsyncClient() as client:
                for chat_id in chat_ids_list:
                    response = await client.post(
                        f"{telegram_api_url}/sendMessage", json={"chat_id": chat_id.strip(), "text": text}
                    )

                    if response.status_code == 200:
                        logging.info(f"Message sent to {chat_id}")
                    else:
                        logging.error(f"Telegram API error for {chat_id}: {response.text}")

            return aiocoap.Message(code=Code.CONTENT, payload=b"Messages sent successfully")

        except Exception as e:
            logging.exception("Exception occurred while processing request")
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )


class CoAPResourceGet(resource.Resource):
    """CoAP Resource to handle GET requests"""
    async def render_post(self, request):
        try:
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            telegram_api_url = data.get("url", "").strip()
            telegram_bot_token = data.get("token", "").strip()

            telegram_api_url = re.sub(r"[^\x20-\x7E]", "", telegram_api_url)
            telegram_bot_token = re.sub(r"[^\x20-\x7E]", "", telegram_bot_token)

            if not telegram_api_url or not telegram_bot_token:
                logging.error("Missing required fields in request")
                logging.error(f"url='{telegram_api_url}', bot_token=[HIDDEN]'")
                return aiocoap.Message(code=Code.BAD_REQUEST, payload=b"Missing required fields")

            async with httpx.AsyncClient() as client:
                response = await client.get(f"{telegram_api_url}{telegram_bot_token}/getUpdates")
                if response.status_code == 200:
                    #data = response.json()
                    #CoAPResource.messages = [update["message"]["text"] for update in data.get("result", []) if "message" in update]
                    logging.info(f"Retrieved messages: {CoAPResource.messages}")
                    return aiocoap.Message(code=Code.CONTENT, payload=json.dumps(response.json()).encode("utf-8"))
                else:
                    logging.error(f"Failed to fetch updates: {response.text}")
                    return aiocoap.Message(code=Code.INTERNAL_SERVER_ERROR, payload=b"Failed to fetch updates")

        except Exception as e:
            logging.exception("Exception occurred while fetching updates")
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )


async def heartbeat():
    """Periodically logs an INFO message every 15 minutes to confirm the server is running."""
    while True:
        logging.info("Server is still running...")
        await asyncio.sleep(900)


async def coap_server():
    """Start CoAP server on COAP_SERVER_IP."""
    logging.info(f"Starting CoAP server on coap://[{coap_server_ip}]:5683")

    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())
    root.add_resource(('update',), CoAPResourceGet())

    await asyncio.gather(
        aiocoap.Context.create_server_context(root, bind=(coap_server_ip, 5683)),
        heartbeat()
    )


if __name__ == "__main__":
    logging.info("Starting CoAP server script...")
    asyncio.run(coap_server())
