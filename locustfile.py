from locust import FastHttpUser, task, between, events
import random

class MyUser(FastHttpUser):
    # Define the wait time between requests
    # wait_time = between(1, 3)

    # Define the mock data to be used for GET and POST requests
    data = [{
        "method": "POST" if i % 2 == 0 else "GET",
        "path": "/resources/" + str(int(i/2)),
        "body": "lorem ipsum dolor sit amet",
    } for i in range(100)]

    @task(1)
    def get_request(self):
        # Select a random GET request from the mock data
        request_data = random.choice([d for d in self.data if d["method"] == "GET"])
        with self.client.get(request_data["path"], catch_response=True) as resp:
            if resp.status_code == 404:
                resp.success()
            else:
                resp.raise_for_status()

    @task(1)
    def post_request(self):
        # Select a random POST request from the mock data
        request_data = random.choice([d for d in self.data if d["method"] == "POST"])
        if "body" in request_data:
            self.client.post(request_data["path"], json=request_data["body"])
        else:
            self.client.post(request_data["path"])
