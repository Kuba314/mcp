from PyQt5.QtCore import QUrl
from PyQt5.QtWidgets import QMainWindow, QApplication
from PyQt5.QtWebEngineWidgets import QWebEngineView

import requests
import sys


class MainWindow(QMainWindow):
    def __init__(self, client_id: str, redirect_uri: str, on_client_code_retrieved):
        super(MainWindow, self).__init__()

        oauth_url: str = (
            f"https://login.live.com/oauth20_authorize.srf?client_id={client_id}"
            f"&response_type=code&redirect_uri={redirect_uri}"
            f"&scope=XboxLive.signin%20offline_access"
        )

        self.browser = QWebEngineView()
        self.browser.setUrl(QUrl(oauth_url))
        self.browser.urlChanged.connect(self.on_url_change)

        self.on_client_code_retrieved = on_client_code_retrieved

        self.setWindowTitle("Microsoft Auth")
        self.setMinimumSize(700, 600)
        self.setCentralWidget(self.browser)

        self.show()

    def on_url_change(self, url: QUrl):
        url_string: str = url.toString()
        if "?code=" not in url_string:
            return

        client_code = url_string.split("?code=")[1]
        if "&" in client_code:
            client_code = client_code[: client_code.index("&")]
        self.on_client_code_retrieved(client_code)


def perform_oauth(client_id: str, redirect_uri: str) -> str:
    class OAuthResult:
        client_code: str

    result: OAuthResult = OAuthResult()

    def on_client_code_retrieved(client_code):
        result.client_code = client_code
        window.close()

    app = QApplication(sys.argv)
    window = MainWindow(client_id, redirect_uri, on_client_code_retrieved)
    app.exec_()

    return result.client_code


def retrieve_access_token(
    client_id: str, client_secret: str, client_code: str, redirect_uri: str
) -> tuple[str, str]:
    url = "https://login.live.com/oauth20_token.srf"
    data = {
        "client_id": client_id,
        # "client_secret": client_secret,
        "code": client_code,
        "grant_type": "authorization_code",
        "redirect_uri": redirect_uri,
    }
    headers = {"Content-Type": "application/x-www-form-urlencoded"}
    r = requests.post(url, data=data, headers=headers)
    r.raise_for_status()
    resp = r.json()
    return (resp["access_token"], resp["refresh_token"])


def refresh_access_token(
    client_id: str, client_secret: str, refresh_token: str, redirect_uri: str
) -> tuple[str, str]:
    url = "https://login.live.com/oauth20_token.srf"
    data = {
        "client_id": client_id,
        # "client_secret": client_secret,
        "refresh_token": refresh_token,
        "grant_type": "refresh_token",
        "redirect_uri": redirect_uri,
    }
    headers = {"Content-Type": "application/x-www-form-urlencoded"}
    r = requests.post(url, data=data, headers=headers)
    r.raise_for_status()
    resp = r.json()
    return (resp["access_token"], resp["refresh_token"])


def auth_xbl(access_token: str) -> tuple[str, str]:
    url = "https://user.auth.xboxlive.com/user/authenticate"
    data = {
        "Properties": {
            "AuthMethod": "RPS",
            "SiteName": "user.auth.xboxlive.com",
            "RpsTicket": f"d={access_token}",
        },
        "RelyingParty": "http://auth.xboxlive.com",
        "TokenType": "JWT",
    }
    headers = {"Content-Type": "application/json", "Accept": "application/json"}
    r = requests.post(url, data=data, headers=headers)
    r.raise_for_status()
    resp = r.json()
    return (resp["Token"], resp["DisplayClaims"]["xui"][0]["uhs"])


def auth_xsts(xbl_token: str) -> str:
    url = "https://xsts.auth.xboxlive.com/xsts/authorize"
    data = {
        "Properties": {
            "SandboxId": "RETAIL",
            "UserTokens": [
                xbl_token,
            ],
        },
        "RelyingParty": "rp://api.minecraftservices.com/",
        "TokenType": "JWT",
    }
    headers = {"Content-Type": "application/json", "Accept": "application/json"}
    r = requests.post(url, data=data, headers=headers)
    r.raise_for_status()
    print(r.status_code, r.text)
    resp = r.json()
    print(resp)
    if "XErr" in resp:
        err_code = resp["XErr"]
        if err_code == 2148916233:
            raise Exception("No xbox account")
        elif err_code == 2148916235:
            raise Exception("Xbox not available in country")
        elif err_code == 2148916238:
            raise Exception("Child not allowed to play")
        else:
            raise Exception("Can't auth with xsts")

    return resp["Token"]


def main() -> None:
    client_id: str = "75267598-be03-43bd-8055-f3c9e320cc06"
    client_secret: str = "CCN7Q~Pr4hAYTyMiYgh3IMW4~5BxjdMwRc88j"
    redirect_uri: str = "https://login.microsoftonline.com/common/oauth2/nativeclient"
    # redirect_uri = "https://login.live.com/oauth20_desktop.srf"

    # client_id = "54473e32-df8f-42e9-a649-9419b0dab9d3"
    # redirect_uri = "https://mccteam.github.io/redirect.html"

    # redirect_uri = 'http://localhost:25453'

    client_code = perform_oauth(client_id, redirect_uri)
    print("client_code:", client_code)

    access_token, refresh_token = retrieve_access_token(
        client_id, client_secret, client_code, redirect_uri
    )
    print("access_token:", access_token)
    print("refresh_token:", refresh_token)

    # access_token, refresh_token = refresh_access_token(client_id, client_secret, refresh_token, redirect_uri)
    # print('access_token:', access_token)
    # print('refresh_token:', refresh_token)

    xbl_token, user_hash = auth_xbl(access_token)
    print("xbl_token:", xbl_token)
    print("user_hash:", user_hash)

    xsts_token = auth_xsts(xbl_token)
    print("xsts_token:", xsts_token)


if __name__ == "__main__":
    main()
