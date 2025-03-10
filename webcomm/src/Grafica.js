import { useState, useEffect } from 'react';
import { Bar, Line } from 'react-chartjs-2';
import { Chart as ChartJS, CategoryScale } from 'chart.js/auto';
import supabase from './supabase';

ChartJS.register(CategoryScale);

const Grafica = () => {
    const [datos, setDatos] = useState([]);

    useEffect(() => {
        const fetchDatos = async () => {
        const { data, error } = await supabase
            .from('sensor_readings')
            .select('*')
            .order('created_at', { ascending: true });

        if (error) console.error(error);
        else setDatos(data);
        };

        fetchDatos();
    }, []);

    // Preparar datos para la gráfica de temperatura
    const datosTemperatura = {
        labels: datos.map(item => item.created_at),
        datasets: [
        {
            label: 'Temperatura',
            data: datos.map(item => item.temperature),
            backgroundColor: 'rgba(255, 99, 132, 0.6)',
        },
        ],
    };

    // Preparar datos para la gráfica de humedad
    const datosHumedad = {
        labels: datos.map(item => item.created_at),
        datasets: [
        {
            label: 'Humedad',
            data: datos.map(item => item.humidity),
            backgroundColor: 'rgba(54, 162, 235, 0.6)',
        },
        ],
    };

    return (
        <div className="App">
        <h1>Lecturas de Sensores</h1>
        <div style={{ marginBottom: '20px' }}>
            <h2>Temperatura</h2>
            <Bar data={datosTemperatura} />
        </div>
        <div>
            <h2>Humedad</h2>
            <Line data={datosHumedad} />
        </div>
        </div>
    );
};

export default Grafica;