const WebSocket = require('ws');
const express = require('express');
const app = express();
const PORT = process.env.PORT || 3000;

// Serve the web UI
app.use(express.static('../web'));
app.listen(PORT, () => console.log(`HTTP server running on port ${PORT}`));

// WebSocket Server
const wss = new WebSocket.Server({ port: 8080 }, () => {
  console.log("WebSocket server running on port 8080");
});

// Keep track of connected clients
const clients = new Set();

wss.on('connection', (ws) => {
  console.log("New WebSocket connection");
  clients.add(ws);

  ws.on('message', (msg) => {
    console.log("Received:", msg.toString());

    // Broadcast to all ESP32 clients
    for (let client of clients) {
      if (client.readyState === WebSocket.OPEN) {
        client.send(msg.toString());
      }
    }
  });

  ws.on('close', () => {
    clients.delete(ws);
    console.log("Client disconnected");
  });
});
