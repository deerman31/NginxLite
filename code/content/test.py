from locust import HttpUser, task

class SimpleUser(HttpUser):
    @task
    def get_request(self):
        # ここでテストしたいURLにGETリクエストを送ります。
        # 例: self.client.get("/index.html")
        self.client.get("/index.html")

