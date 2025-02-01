import argparse
import asyncio
import os

import aiocoap
import httpx
from aiocoap import resource, Code
from dotenv import load_dotenv

# Get Token from .env file
load_dotenv(".env")
telegram_bot_token = os.getenv("TELEGRAM_BOT_TOKEN")
telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}"


class CoAPResource(resource.Resource):
    """CoAP Resource to handle POST requests"""

    async def render_post(self, request):
        try:
            # Decode and parse the payload
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            chat_id = data.get("chat_id")
            text = data.get("text")
            print(f"Text: '{text}' and id: '{chat_id}'")

            if not chat_id or not text:
                return aiocoap.Message(
                    code=Code.BAD_REQUEST, payload=b"Missing chat_id or text"
                )

            # Send the message to Telegram
            async with httpx.AsyncClient() as client:
                response = await client.post(
                    f"{telegram_api_url}/sendMessage", json={"chat_id": chat_id, "text": text}
                )

            if response.status_code == 200:
                return aiocoap.Message(
                    code=Code.CONTENT, payload=b"Message sent successfully"
                )
            else:
                return aiocoap.Message(
                    code=Code.BAD_GATEWAY,
                    payload=f"Telegram API error: {response.text}".encode("utf-8"),
                )

        except Exception as e:
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )


async def coap_server(server_url):
    BLUE = '\033[34m'
    RESET = '\033[0m'
    # Create CoAP context and add the resource
    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())

    await aiocoap.Context.create_server_context(root, bind=(server_url, 5683))
    print(f"{BLUE}INFO{RESET}:\t  CoAP server running on coap://{server_url}:5683")
    await asyncio.get_running_loop().create_future()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Start the websocket.")
    parser.add_argument("-u", "--url", metavar='', type=str, default="0.0.0.0",
                        help="Local IP address where the uvicorn server is started (default: 0.0.0.0)")
    args = parser.parse_args()

    load_dotenv(".env")  # Laden der .env-Datei für das Telegram-Token
    asyncio.run(coap_server(args.url))
