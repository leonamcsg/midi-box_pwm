import socket

# Configurações do ESP8266
ESP_IP = "192.168.4.1"  # <--- COLOQUE O IP QUE APARECE NO SERIAL MONITOR
PORT = 8888

# Cria o socket UDP (AF_INET = IPv4, SOCK_DGRAM = UDP)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print(f"Conectado ao MIDI Box em {ESP_IP}:{PORT}")
print("Digite a frequência (ou 'sair' para encerrar):")

try:
    while True:
        msg = input("Frequência (Hz) > ")
        
        if msg.lower() == 'sair':
            break
            
        # Envia a string codificada em bytes
        sock.sendto(msg.encode(), (ESP_IP, PORT))
        
except KeyboardInterrupt:
    print("\nEncerrando...")
finally:
    sock.close()