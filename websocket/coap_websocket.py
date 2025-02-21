import asyncio
import os
import logging

import aiocoap
import httpx
from aiocoap import resource, Code
import re
import time

# Only allow for WARNING logging from automatic loggers
logging.getLogger("httpx").setLevel(logging.WARNING)
logging.getLogger("urllib3").setLevel(logging.WARNING)
logging.getLogger("urllib3.connectionpool").setLevel(logging.WARNING)

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
start_time = int(time.time())


class CoAPResource(resource.Resource):
    """CoAP Resource to handle telegram POST requests"""
    async def render_post(self, request):
        try:
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            telegram_api_url = data.get("url", "").strip()
            telegram_bot_token = data.get("token", "").strip()
            chat_ids = data.get("chat_ids", "").strip()
            text = data.get("text", "").strip()
            text = text.replace("\x00", "").replace("\n", "")

            if not telegram_api_url or not telegram_bot_token or not chat_ids or not text:
                logging.error("Missing required fields in request")
                logging.error(f"url='{telegram_api_url}', bot_token=[HIDDEN], chat_ids={chat_ids}, text='{text}'")
                return aiocoap.Message(code=Code.BAD_REQUEST, payload=b"Missing required fields")


            logging.info(f"Received message request: url='{telegram_api_url}', bot_token=[HIDDEN], chat_ids={chat_ids}, text='{text}'")
            telegram_api_url = f"{telegram_api_url}{telegram_bot_token}"
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


class CoAPResourceGet(resource.Resource):
    """CoAP Resource to handle telegram GET requests"""

    def __init__(self):
        super().__init__()
        self.last_update = start_time                               # Track the system time
        self.chats = {}                                             # Store chats as a list [{first_name_1, chat_id_1},{first_name_2, chat_id_3},...] <- max 10
        self.latest_values = {"interval": 2, "feedback": 0}         # Store latest values
        self.password = "password12"                                # "Secret"
        self.update_storage_threshold = 50                          # The maximum number of updates stored on server

    async def render_post(self, request):
        try:
            payload = request.payload.decode("utf-8")
            data = {k: v for k, v in (item.split("=") for item in payload.split("&"))}

            telegram_api_url = data.get("url", "").strip()
            telegram_bot_token = data.get("token", "").strip()

            telegram_api_url = re.sub(r"[^\x20-\x7E]", "", telegram_api_url)
            telegram_bot_token = re.sub(r"[^\x20-\x7E]", "", telegram_bot_token)

            if not telegram_api_url or not telegram_bot_token:
                logging.error("Missing required fields in request")
                return aiocoap.Message(code=Code.BAD_REQUEST, payload=b"Missing required fields")

            # Step 3: Make the API call to get updates from Telegram
            async with httpx.AsyncClient() as client:
                response = await client.get(f"{telegram_api_url}{telegram_bot_token}/getUpdates")

                if response.status_code != 200:
                    logging.error(f"Failed to fetch updates: {response.text}")
                    return aiocoap.Message(code=Code.INTERNAL_SERVER_ERROR, payload=b"Failed to fetch updates")

                data = response.json()
                updated_values = {}
                removal_chat_id = None
                added_chats = {}

                # Step 4: Process Telegram updates (without update_id)
                for update in data.get("result", []):
                    message = update.get("message", {})
                    text = message.get("text", "").strip()
                    chat_id = message.get("chat", {}).get("id", "")
                    first_name = message.get("chat", {}).get("first_name", "")
                    timestamp = message.get("date", None)

                    if not text:
                        continue

                    # Handle new user registration
                    if chat_id and first_name and chat_id not in self.chats:
                        added_chats[chat_id] = first_name

                    # Handle "remove me"
                    if text.lower() == "remove me":
                        if chat_id in self.chats:
                            removal_chat_id = chat_id
                            del self.chats[chat_id]
                            await self._notify_user(telegram_api_url, telegram_bot_token, chat_id, "You have been removed.")
                        else:
                            await self._notify_user(telegram_api_url, telegram_bot_token, chat_id, "You are not in the list.")
                        continue

                    # Process "config" messages
                    if not text.lower().startswith("config "):
                        continue

                    # Extract message components
                    parts = text.split(" ", 3)
                    if len(parts) < 4:
                        continue

                    _, password, name, value = parts

                    if password != self.password:
                        logging.warning(f"Invalid password received: {password}")
                        #await self._notify_user(telegram_api_url, telegram_bot_token, chat_id, "Invalid password.")
                        continue

                    if timestamp and int(timestamp) > self.last_update:
                        # Validate input values before updating
                        if name == "interval":
                            try:
                                interval_value = int(value)
                                if not (1 <= interval_value <= 120):
                                    raise ValueError
                                if interval_value != self.latest_values["interval"]:
                                    updated_values["interval"] = interval_value
                            except ValueError:
                                await self._notify_user(telegram_api_url, telegram_bot_token, chat_id, "Invalid interval. Must be between 1 and 120.")
                                continue

                        elif name == "feedback":
                            if value not in ["0", "1"]:
                                await self._notify_user(telegram_api_url, telegram_bot_token, chat_id, "Invalid feedback. Must be 0 or 1.")
                                continue
                            if value != self.latest_values["feedback"]:
                                updated_values["feedback"] = value

                # Step 5: If a change occurred, send update
                if updated_values or added_chats or removal_chat_id:
                    print(f"Telegram timestamp: {timestamp}, self.timestamp: {self.last_update}")
                    self._fancy_logging(self.latest_values, removal_chat_id, added_chats)
                    self.latest_values.update(updated_values)  # Update latest stored values
                    if removal_chat_id:
                        del added_chats[removal_chat_id]
                    self.chats.update(added_chats) # Update chat IDs
                    self.last_update = int(time.time())  # Update the timestamp as soon as a change occurs
                    compact_message = self._encode_message(updated_values, removal_chat_id, added_chats)
                    return aiocoap.Message(code=Code.CONTENT, payload=compact_message)

                # Step 6: If nothing changed, return "No Updates"
                logging.info("No changes detected, nothing sent via CoAP.")
                return aiocoap.Message(code=Code.VALID, payload=b"No Updates")

        except Exception as e:
            logging.exception("Exception occurred while fetching updates")
            return aiocoap.Message(
                code=Code.INTERNAL_SERVER_ERROR,
                payload=f"Internal server error: {str(e)}".encode("utf-8"),
            )

    def _encode_message(self, updates, removal_chat_id, added_chats):
        """Encodes updates into a compact byte string for CoAP"""
        encoded_list = []

        if "interval" in updates:
            encoded_list.append(f"i{updates['interval']}")  # Use "i" for interval

        if "feedback" in updates:
            encoded_list.append(f"f{updates['feedback']}")  # Use "f" for feedback

        if added_chats:  # Encode chats in the format "first_name1:chat_id_1;first_name_2:chat_id_2;..."
            chat_string = ";".join([f"{first_name}:{chat_id}" for chat_id, first_name in added_chats.items()])
            encoded_list.append(chat_string)

        if removal_chat_id:
            encoded_list.append(f"{removal_chat_id}")

        encoded_string = ";".join(encoded_list)  # Separate multiple updates with ";"
        return encoded_string.encode("utf-8")

    def _remove_user(self, chat_id):
        """Remove a user by chat_id from chats"""
        if chat_id in self.chats:
            del self.chats[chat_id]
            return True
        return False

    def _fancy_logging(self, updates, removal_chat_id, added_chats):
        """Make the logging of changes fancy"""
        log_message = "========== APPLIED UPDATES =========="
        if updates:
            for key, value in updates.items():
                log_message += f"\n- {key.capitalize()}: {value}"
        if self.chats:
            log_message += "\nRegistered Users:"
            for chat_id, first_name in self.chats.items():
                log_message += f"\n- {first_name} (Chat ID: {chat_id})"
        if added_chats:
            log_message += f"\nNew User(s):"
            for chat_id, first_name in added_chats.items():
                log_message += f"\n- {first_name} (Chat ID: {chat_id})"
        if removal_chat_id:
            log_message += f"\nRemoved User (Chat ID): {removal_chat_id}"
        log_message += "\n====================================="
        logging.info(log_message)

    async def _notify_user(self, api_url, bot_token, chat_id, message):
        """Handle telegram confirmation / user feedback messages"""
        try:
            async with httpx.AsyncClient() as client:
                url = f"{api_url}{bot_token}/sendMessage"
                payload = {"chat_id": chat_id, "text": message}
                await client.post(url, json=payload)
        except Exception as e:
            logging.error(f"Failed to send notification to {chat_id}: {e}")

async def heartbeat():
    """Periodically logs an INFO message every 15 minutes to confirm the server is running."""
    while True:
        logging.info("Server is still running...")
        await asyncio.sleep(900)


async def coap_server():
    """Start CoAP server on COAP_SERVER_IP."""
    logging.info(f"Starting CoAP server on coap://[{coap_server_ip}]:5683")

    root = resource.Site()
    root.add_resource(('.well-known/core',), resource.WKCResource(root.get_resources_as_linkheader))
    root.add_resource(('message',), CoAPResource())
    root.add_resource(('update',), CoAPResourceGet())

    await asyncio.gather(
        aiocoap.Context.create_server_context(root, bind=(coap_server_ip, 5683)),
        heartbeat()
    )


if __name__ == "__main__":
    logging.info("Starting CoAP server script...")
    asyncio.run(coap_server())
