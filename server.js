const express = require("express");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

let espClient = null; // store ESP32 connection

wss.on("connection", (ws) => {
    console.log("New client connected");

    ws.on("message", (msg) => {
        if(msg === "ESP32_CONNECT") { 
            espClient = ws; 
            console.log("ESP32 connected"); 
            return; 
        }

        // Forward commands from browser to ESP32
        if(espClient && ws !== espClient) espClient.send(msg);
    });

    ws.on("close", () => { 
        if(ws === espClient) espClient = null; 
        console.log("Client disconnected");
    });
});

app.get("/", (req, res) => res.send("ESP32 Tank WebSocket Server Running"));

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => console.log(`Server running on port ${PORT}`));
