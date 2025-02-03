import asyncio
import os
import logging

import aiocoap
import httpx
from aiocoap import resource, Code

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

            telegram_bot_token = data.get("telegram_bot_token")
            chat_ids = data.get("chat_ids")
            text = data.get("text")

            if not telegram_bot_token:
                logging.error(f"Telegram bot token is missing, failed to send message")
                return aiocoap.Message(
                    code=Code.BAD_REQUEST, payload=b"Missing bot token"
                )
            if not chat_ids:
                logging.error(f"Chat id(s) not available, failed to send message")
                return aiocoap.Message(
                    code=Code.BAD_REQUEST, payload=b"Missing chat_ids"
                )
            if not text:
                logging.error(f"Chat text is missing, failed to send message")
                return aiocoap.Message(
                    code=Code.BAD_REQUEST, payload=b"Missing text"
                )

            logging.info(f"Received message request: bot_token={telegram_bot_token}, chat_ids={chat_ids}, text='{text}'")
            telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}"
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

async def coap_server():
    """Start CoAP server on COAP_SERVER_IP."""
    logging.info(f"Starting CoAP server on coap://[{coap_server_ip}]:5683")

    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())

    await aiocoap.Context.create_server_context(root, bind=(coap_server_ip, 5683))
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    logging.info("Starting CoAP server script...")
    asyncio.run(coap_server())
