import asyncio
import os
import logging

import aiocoap
import httpx
from aiocoap import resource, Message, Code, Type

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

import asyncio

class CoAPResource(resource.Resource):
    """CoAP Resource to handle POST requests"""

    async def send_telegram_messages(self, telegram_api_url, chat_ids_list, text):
        """Send messages asynchronously to prevent CoAP delays"""
        async with httpx.AsyncClient() as client:
            for chat_id in chat_ids_list:
                response = await client.post(
                    f"{telegram_api_url}/sendMessage", json={"chat_id": chat_id.strip(), "text": text}
                )

                if response.status_code == 200:
                    logging.info(f"Message sent to {chat_id}")
                else:
                    logging.error(f"Telegram API error for {chat_id}: {response.text}")

    async def render_post(self, request):
        try:
            if request.mtype == Type.CON:
                ack = Message(code=Code.EMPTY, mtype=Type.ACK, mid=request.mid)
                await request.reply(ack)

            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            telegram_api_url = data.get("url")
            telegram_bot_token = data.get("token")
            chat_ids = data.get("chat_ids")
            text = data.get("text")

            if not telegram_api_url or not telegram_bot_token or not chat_ids or not text:
                logging.error("Missing required fields in request")
                return aiocoap.Message(code=Code.BAD_REQUEST, payload=b"Missing required fields")

            logging.info(f"Received message request: url='{telegram_api_url}', bot_token={telegram_bot_token}, chat_ids={chat_ids}, text='{text}'")

            telegram_api_url = f"{telegram_api_url}{telegram_bot_token}"
            chat_ids_list = chat_ids.split(",")

            asyncio.create_task(self.send_telegram_messages(telegram_api_url, chat_ids_list, text))

            return aiocoap.Message(code=Code.CONTENT, payload=b"Messages sent successfully")

        except Exception as e:
            logging.exception("Exception occurred while processing request")
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )

async def heartbeat():
    """Periodically logs an INFO message every 1 minute to confirm the server is running."""
    while True:
        logging.info("Server is still running...")
        await asyncio.sleep(60)

async def coap_server():
    """Start CoAP server on COAP_SERVER_IP."""
    logging.info(f"Starting CoAP server on coap://[{coap_server_ip}]:5683")

    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())

    await asyncio.gather(
        aiocoap.Context.create_server_context(root, bind=(coap_server_ip, 5683)),
        heartbeat()
    )


if __name__ == "__main__":
    logging.info("Starting CoAP server script...")
    asyncio.run(coap_server())
