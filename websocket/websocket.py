import argparse
import asyncio
import logging
import os

import aiocoap
import aiocoap.resource as resource
from aiocoap.numbers.codes import Code
import httpx
from dotenv import load_dotenv
from fastapi import FastAPI, WebSocket, Request
import uvicorn


app = FastAPI()

# Get Token from .env file
load_dotenv(".env")
telegram_bot_token = os.getenv("TELEGRAM_BOT_TOKEN")
telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}"


@app.post("/send_message/")
async def send_message(request: Request):
    try:
        payload = await request.json()
        chat_id = payload["chat_id"]
        text = payload["text"]

        async with httpx.AsyncClient() as client:
            response = await client.post(
                f"{telegram_api_url}/sendMessage",
                json={"chat_id": chat_id, "text": text},
            )

        return {"status": "success", "telegram_response": response.json()}
    except Exception as e:
        return {"status": "error", "message": str(e)}


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            data = await websocket.receive_json()
            chat_id = data.get("chat_id")
            text = data.get("text")

            async with httpx.AsyncClient() as client:
                response = await client.post(
                    f"{telegram_api_url}/sendMessage",
                    json={"chat_id": chat_id, "text": text},
                )

            await websocket.send_json({"status": "success", "telegram_response": response.json()})

    except Exception as e:
        await websocket.send_json({"status": "error", "message": str(e)})
        await websocket.close()


@app.get("/get_updates")
async def get_updates(offset: int = None, timeout: int = 0):
    try:
        params = {"timeout": timeout}
        if offset is not None:
            params["offset"] = offset

        async with httpx.AsyncClient() as client:
            response = await client.get(
                f"{telegram_api_url}/getUpdates", params=params
            )

        print(response.json)
        return {"status": "success", "telegram_response": response.json()}
    except Exception as e:
        return {"status": "error", "message": str(e)}


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
    # await aiocoap.Context.create_server_context(root, bind=("fe80::a41d:b3f5:5212:392f%enp0s3", 5683))
    print(f"{BLUE}INFO{RESET}:\t  CoAP server running on coap://{server_url}:5683")
    await asyncio.sleep(3600)  # Server runs for 1 hour


# Funktion zum Starten beider Server
async def start_servers(server_url):
    # CoAP-Server in separatem Task starten
    coap_task = asyncio.create_task(coap_server(server_url))

    # Uvicorn für FastAPI starten
    uvicorn_config = uvicorn.Config(app, host=server_url, port=8000)
    uvicorn_server = uvicorn.Server(uvicorn_config)
    fastapi_task = asyncio.create_task(uvicorn_server.serve())

    # Beide Server gleichzeitig laufen lassen
    await asyncio.gather(coap_task, fastapi_task)

logging.basicConfig(level=logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)

# FastAPI und CoAP-Server starten
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Start the websocket.")
    parser.add_argument("-u", "--url", metavar='', type=str, default="0.0.0.0",
                        help="Local IP address where the uvicorn server is started (default: 0.0.0.0)")
    args = parser.parse_args()
    uvicorn_url = args.url
    load_dotenv(".env")  # Laden der .env-Datei für das Telegram-Token
    asyncio.run(start_servers(uvicorn_url))