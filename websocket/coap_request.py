import asyncio
from aiocoap import *

async def main():
    # The server address and port
    uri = "coap://127.0.0.2:5683/message"

    # The payload as a byte string
    payload = b"chat_id=7779371199&text=123456789 T E S T"

    # Create a CoAP message
    request = Message(code=POST, uri=uri, payload=payload)

    # Create a CoAP client context
    protocol = await Context.create_client_context()

    try:
        # Send the request and wait for the response
        response = await protocol.request(request).response
        print(f"Response Code: {response.code}")
        print(f"Response Payload: {response.payload.decode()}")
    except Exception as e:
        print(f"Failed to send request: {e}")

# Run the async function
asyncio.run(main())
