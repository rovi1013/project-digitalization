from fastapi import FastAPI, WebSocket, Request
import httpx

app = FastAPI()

TELEGRAM_BOT_TOKEN = "7975746972:AAEJo471A-1vMk76RU0-1HY0epnl0IimtWE"
TELEGRAM_API_URL = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}"

# Boot server with "uvicorn websocket:app --reload


@app.post("/send_message/")
async def send_message(request: Request):
    try:
        payload = await request.json()
        chat_id = payload["chat_id"]
        text = payload["text"]

        async with httpx.AsyncClient() as client:
            response = await client.post(
                f"{TELEGRAM_API_URL}/sendMessage",
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
                    f"{TELEGRAM_API_URL}/sendMessage",
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
                f"{TELEGRAM_API_URL}/getUpdates", params=params
            )

        print(response.json)
        return {"status": "success", "telegram_response": response.json()}
    except Exception as e:
        return {"status": "error", "message": str(e)}

