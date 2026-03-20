# BACKEND FOR THE DASHBOARD

## Requirement Libs
- Docker
- Sequelize
- Dotenv
- Mqtt

## Folder Structure
backend/
├── src/
│   ├── config/
│   │   └── database.js    
│   ├── models/
│   │   └── sensorModel.js  
│   ├── services/
│   │   └── mqttService.js  
│   └── app.js             
├── .env                   
├── Dockerfile             
├── package.json           
└── README.md

## Ports:
- Backend: 5000
- Mysql: 3306

## Backend Server
- [https://localhost:5000](https://localhost:5000)