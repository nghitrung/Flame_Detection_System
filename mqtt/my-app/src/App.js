import React, { useState, useEffect } from 'react';
import { Flame, Wind, AlertTriangle } from 'lucide-react';

const API_URL = 'http://localhost:5000/api/sensors/latest';

function App() {
  const [data, setData] = useState({
    flame1: 0, flame2: 0, smoke1: 0, smoke2: 0
  });
  const [loading, setLoading] = useState(true);

  // Hàm lấy dữ liệu từ Backend
  const fetchSensorData = async () => {
    try {
      const response = await fetch(API_URL);
      const json = await response.json();
      if (json) setData(json);
      setLoading(false);
    } catch (error) {
      console.error("Lỗi kết nối Backend:", error);
    }
  };

  useEffect(() => {
    fetchSensorData();
    const interval = setInterval(fetchSensorData, 2000); // Cập nhật mỗi 2 giây
    return () => clearInterval(interval);
  }, []);

  // Hàm xác định mức độ nguy hiểm (ngưỡng giả định: > 500)
  const isDanger = (val) => val > 500;

  const SensorCard = ({ title, value, icon: Icon, color, unit = "" }) => (
    <div style={{
      background: '#fff',
      padding: '20px',
      borderRadius: '15px',
      boxShadow: '0 4px 6px rgba(0,0,0,0.1)',
      borderLeft: `10px solid ${value > 500 ? '#ff4d4d' : color}`,
      textAlign: 'center',
      minWidth: '180px'
    }}>
      <Icon size={32} color={value > 500 ? '#ff4d4d' : color} />
      <h3 style={{ color: '#555', margin: '10px 0' }}>{title}</h3>
      <div style={{ fontSize: '32px', fontWeight: 'bold' }}>
        {value}{unit}
      </div>
      {value > 500 && <p style={{ color: '#ff4d4d', fontSize: '12px', fontWeight: 'bold' }}>⚠️ CẢNH BÁO CAO</p>}
    </div>
  );

  if (loading) return <div style={{ textAlign: 'center', marginTop: '50px' }}>Đang kết nối hệ thống...</div>;

  return (
    <div style={{ backgroundColor: '#f4f7f6', minHeight: '100vh', padding: '40px', fontFamily: 'Segoe UI, sans-serif' }}>
      <header style={{ textAlign: 'center', marginBottom: '40px' }}>
        <h1 style={{ color: '#2c3e50' }}>Hệ Thống Giám Sát Phòng Cháy Real-time</h1>
        <p style={{ color: '#7f8c8d' }}>Dữ liệu từ bảng MySQL: <b>data_sensors</b></p>
      </header>

      {/* Khu vực hiển thị các Card chỉ số */}
      <div style={{ display: 'flex', flexWrap: 'wrap', justifyContent: 'center', gap: '20px' }}>
        <SensorCard title="Lửa 1 (Flame)" value={data.flame1} icon={Flame} color="#e67e22" />
        <SensorCard title="Lửa 2 (Flame)" value={data.flame2} icon={Flame} color="#e67e22" />
        <SensorCard title="Khói 1 (Smoke)" value={data.smoke1} icon={Wind} color="#95a5a6" />
        <SensorCard title="Khói 2 (Smoke)" value={data.smoke2} icon={Wind} color="#95a5a6" />
      </div>

      {/* Thông báo tổng quát */}
      {(data.flame1 > 500 || data.smoke1 > 500) && (
        <div style={{
          marginTop: '40px',
          background: '#ff4d4d',
          color: 'white',
          padding: '20px',
          borderRadius: '10px',
          textAlign: 'center',
          animation: 'pulse 1s infinite'
        }}>
          <AlertTriangle size={48} style={{ marginBottom: '10px' }} />
          <h2>NGUY HIỂM! PHÁT HIỆN DẤU HIỆU CHÁY</h2>
        </div>
      )}
    </div>
  );
}

export default App;