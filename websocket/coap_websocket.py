import asyncio
import os
import logging

import aiocoap
import httpx
from aiocoap import resource, Code
from dotenv import load_dotenv

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

# Load environment variables
load_dotenv(".env")
telegram_bot_token = os.getenv("TELEGRAM_BOT_TOKEN")
telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}"

# Get IPv6 address from systemd environment
coap_server_ip = os.getenv("COAP_SERVER_IP", "::1")  # Default to localhost (::1) if unset
logging.info(f"Using CoAP server IP: {coap_server_ip}")


class CoAPResource(resource.Resource):
    """CoAP Resource to handle POST requests"""

    async def render_post(self, request):
        try:
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            chat_id = data.get("chat_id")
            text = data.get("text")
            logging.info(f"Received message request: chat_id={chat_id}, text='{text}'")

            if not chat_id or not text:
                return aiocoap.Message(
                    code=Code.BAD_REQUEST, payload=b"Missing chat_id or text"
                )

            async with httpx.AsyncClient() as client:
                response = await client.post(
                    f"{telegram_api_url}/sendMessage", json={"chat_id": chat_id, "text": text}
                )

            if response.status_code == 200:
                logging.info("Message sent successfully")
                return aiocoap.Message(
                    code=Code.CONTENT, payload=b"Message sent successfully"
                )
            else:
                logging.error(f"Telegram API error: {response.text}")
                return aiocoap.Message(
                    code=Code.BAD_GATEWAY,
                    payload=f"Telegram API error: {response.text}".encode("utf-8"),
                )

        except Exception as e:
            logging.exception("Exception occurred while processing request")
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )


async def coap_server():
    """Start CoAP server using COAP_SERVER_IP from environment."""
    logging.info(f"Starting CoAP server on coap://[{coap_server_ip}]:5683")

    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())

    await aiocoap.Context.create_server_context(root, bind=(coap_server_ip, 5683))
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    logging.info("Starting CoAP server script...")
    asyncio.run(coap_server())
