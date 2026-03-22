const express = require('express');
const mqtt = require('mqtt');
const mysql = require('mysql2');
const cors = require('cors');

const app = express();
app.use(cors()); 
app.use(express.json());

const db = mysql.createPool({
  host: 'mysql',      // Name service in compose.yaml
  user: 'Flame_Detection_System',      // User in compose.yaml
  password: '123',    
  database: 'data_sensors',
  waitForConnections: true,
  connectionLimit: 10
});

const MQTT_URL = 'mqtt://mosquitto:1883';
const mqttClient = mqtt.connect(MQTT_URL, {
  username: 'Flame_Detection_System',
  password: '123'
});

mqttClient.on('connect', () => {
  console.log('Backend is connected MQTT Broker!');
  mqttClient.subscribe('yolo_uno/sensors/data'); 
});

mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    
    // Mapping to import to the database
    const query = `INSERT INTO data_sensors (flame1, flame2, smoke1, smoke2) VALUES (?, ?, ?, ?)`;
    const values = [
      data.f1 || 0, 
      data.f2 || 0, 
      data.s1 || 0, 
      data.s2 || 0
    ];

    db.query(query, values, (err) => {
      if (err) console.error('Error Insert MySQL:', err.message);
      else console.log('Data have inserted');
    });
  } catch (error) {
    console.error('MQTT Error (need to be a JSON):', message.toString());
  }
});

// API for frontend
app.get('/api/sensors/latest', (req, res) => {
  const query = 'SELECT * FROM data_sensors ORDER BY id DESC LIMIT 1';
  db.query(query, (err, results) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(results[0] || {});
  });
});

app.get('/api/sensors/history', (req, res) => {
  const query = 'SELECT * FROM data_sensors ORDER BY id DESC LIMIT 20';
  db.query(query, (err, results) => {
    if (err) return res.status(500).json({ error: err.message });
    res.json(results.reverse()); 
  });
});

const PORT = 5000;
app.listen(PORT, () => {
  console.log(`Backend is running at http://localhost:${PORT}`);
});