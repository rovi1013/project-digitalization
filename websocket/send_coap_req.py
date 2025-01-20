import asyncio
from aiocoap import *
import argparse

async def main():
    parser = argparse.ArgumentParser(description="Send telegram message via coap.")
    parser.add_argument("-u", "--url", metavar='', type=str, default="0.0.0.0",
                        help="IP address where the uvicorn server is started (default: 0.0.0.0)")
    parser.add_argument("-m", "--msg", metavar='', type=str, default="test",
                        help="Message to send to telegram")
    parser.add_argument("-i", "--id", metavar='', type=str, default="7779371199",
                        help="Chat id of the telegram chat")
    args = parser.parse_args()
    coap_url = args.url
    telegram_msg = args.msg
    chat_id = args.id

    # The server address and port
    uri = f"coap://{coap_url}:5683/message"

    # The payload as a byte string
    payload = b"chat_id=" + chat_id.encode("utf-8") + b"&text=" + telegram_msg.encode("utf-8")
    print(f"The coap message: {payload}")

    # Create a CoAP message
    request = Message(code=POST, uri=uri, payload=payload)

    # Create a CoAP client context
    coap_protocol = await Context.create_client_context()

    try:
        # Send the request and wait for the response
        response = await coap_protocol.request(request).response
        print(f"Response Code: {response.code}")
        print(f"Response Payload: {response.payload.decode()}")
    except Exception as e:
        print(f"Failed to send request: {e}")

# Run the async function
asyncio.run(main())
