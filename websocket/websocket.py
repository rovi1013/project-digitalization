import uvicorn
from fastapi import FastAPI, WebSocket, Request
from dotenv import load_dotenv
import httpx
import os
import argparse

import asyncio
from aiocoap import Context, Message, POST, resource

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

    async def render_post(self, request: Message):
        try:
            # Parse the payload
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            chat_id = data.get("chat_id")
            text = data.get("text")

            print(f"Received Message ID (mid): {request.mid}")

            if not chat_id or not text:
                return Message(code=400, payload=b"Missing chat_id or text")

            # Loggen der `mid`, um den Wert zu sehen
            print(f"Message ID (mid): {request.mid}")

            # Send the message to Telegram
            async with httpx.AsyncClient() as client:
                response = await client.post(
                    f"{telegram_api_url}/sendMessage",
                    json={"chat_id": chat_id, "text": text},
                )

            if response.status_code == 200:
                return Message(code=200, payload=b"Message sent successfully")
            else:
                return Message(code=500, payload=f"Telegram API error: {response.text}".encode("utf-8"))
        except Exception as e:
            return Message(code=500, payload=f"Error: {str(e)}".encode("utf-8"))


async def coap_server(server_url):
    BLUE = '\033[34m'
    RESET = '\033[0m'
    # Create CoAP context and add the resource
    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())

    await Context.create_server_context(root, bind=(server_url, 5683))
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


# FastAPI und CoAP-Server starten
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Start the websocket.")
    parser.add_argument("-u", "--url", metavar='', type=str, default="127.0.0.1",
                        help="Local IP address where the uvicorn server is started (default: 127.0.0.1)")
    args = parser.parse_args()
    uvicorn_url = args.url
    load_dotenv(".env")  # Laden der .env-Datei für das Telegram-Token
    asyncio.run(start_servers(uvicorn_url))