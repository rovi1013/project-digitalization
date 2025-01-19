import asyncio
from aiocoap import *
import argparse

async def main():
    parser = argparse.ArgumentParser(description="Send telegram message via coap.")
    parser.add_argument("-u", "--url", metavar='', type=str, default="0.0.0.0",
                        help="IP address where the uvicorn server is started (default: 0.0.0.0)")
    parser.add_argument("-m", "--msg", metavar='', type=str, default="test",
                        help="Message to send to telegram")
    args = parser.parse_args()
    coap_url = args.url
    telegram_msg = args.msg

    # The server address and port
    uri = f"coap://{coap_url}:5683/message"

    # The payload as a byte string
    payload = b"chat_id=7779371199&text=" + telegram_msg.encode("utf-8")

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
