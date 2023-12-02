import paho.mqtt.client as paho
from paho import mqtt
import re
import matplotlib.pyplot as plt

temperaturas = []  # Lista para armazenar temperaturas
umidades = []  # Lista para armazenar umidades
horarios = []  # Lista para armazenar horários

# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload.decode('utf-8')))
    dados = (str(msg.payload.decode('utf-8')))
    proc_dados(dados)

# using MQTT version 5 here, for 3.1.1: MQTTv311, 3.1: MQTTv31
# userdata is user defined data of any type, updated by user_data_set()
# client_id is the given name of the client
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
client.username_pw_set("", "")
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
client.connect("", 8883)

# setting callbacks, use separate functions like above for better visibility
client.on_subscribe = on_subscribe
client.on_message = on_message

client.subscribe("Temperatura e Umidade", qos=0)

def proc_dados(dados):
    match = re.match(r'\[(\d{2}/\d{2}/\d{2} \d{2}:\d{2}:\d{2}) (\d+\.\d+)°C (\d+\.\d+)%\]', dados)
    
    if match:
        data_hora = match.group(1)
        temperatura = float(match.group(2))
        umidade = float(match.group(3))
        
        # Pega apenas o horário
        hora = data_hora.split()[1]

        # Adiciona os dados às listas
        horarios.append(hora)
        temperaturas.append(temperatura)
        umidades.append(umidade)

        # Atualiza o gráfico
        atualizar_graficos()

def atualizar_graficos():
    plt.close()
    plt.figure(figsize=(10, 8))

    num_pontos = 10  # Número de pontos a serem plotados

    # Determina o intervalo de índices para os últimos 10 pontos
    inicio = max(0, len(horarios) - num_pontos)
    horarios_plot = horarios[inicio:]
    temperaturas_plot = temperaturas[inicio:]
    umidades_plot = umidades[inicio:]

    # Subplot para temperatura
    plt.subplot(2, 1, 1)
    plt.plot(horarios_plot, temperaturas_plot, 'o-', label='Temperatura (°C)')
    plt.xlabel('Horário')
    plt.ylabel('Temperatura (°C)')
    plt.xticks(rotation=45)  # Rotação para melhorar a legibilidade
    plt.legend()
    plt.grid()

    # Subplot para umidade
    plt.subplot(2, 1, 2)
    plt.plot(horarios_plot, umidades_plot, 'o-', label='Umidade (%)')
    plt.xlabel('Horário')
    plt.ylabel('Umidade (%)')
    plt.xticks(rotation=45)  # Rotação para melhorar a legibilidade
    plt.legend()
    plt.grid()

    # Ajusta o layout para evitar sobreposição
    plt.tight_layout()

    # Atualiza o gráfico
    plt.draw()
    plt.pause(0.1)  # Pequeno atraso para permitir a atualização gráfica

# loop_forever for simplicity, here you need to stop the loop manually
# you can also use loop_start and loop_stop
client.loop_forever()