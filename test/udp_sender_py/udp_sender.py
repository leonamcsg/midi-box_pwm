import socket

# Configurações do ESP8266
ESP_IP = "midi-box-esp8266.local" # Tente colocar o IP fixo aqui se o .local falhar
PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(5.0)

def validar_comando(comando):
    """Valida se a string do comando está no formato correto (2, 3 ou 6 parâmetros)."""
    try:
        partes = [p.strip() for p in comando.split(',')]
        qtd = len(partes)
        
        # Agora aceitamos 2, 3 (com duty) ou 6 (completo)
        if qtd not in [2, 3, 6]:
            return False, "Formato incorreto. Use: freq, voice [, duty, start, dur, effect]"
        
        freq = float(partes[0])
        voice = int(partes[1])
        
        if not (0 <= freq <= 20000): 
            return False, "Frequência fora do range (0-20k)"
            
        if voice < 0 or voice > 1: 
            return False, "Voice deve ser 0 ou 1"

        # Se enviou o Duty Cycle (3 ou 6 parâmetros)
        if qtd >= 3:
            duty = float(partes[2])
            if not (0 <= duty <= 100):
                return False, "Duty cycle deve ser entre 0 e 100 (%)"

        # Se enviou o formato completo (6 parâmetros)
        if qtd == 6:
            start = float(partes[3])
            dur = float(partes[4])
            if start < 0 or dur < 0: 
                return False, "Tempo/Duração inválidos"
        
        return True, None
    except ValueError:
        return False, "Valores numéricos inválidos"

print(f"--- MIDI Box Controller ({ESP_IP}:{PORT}) ---")
print("Comandos suportados:")
print("  Simples: freq, voice")
print("  Com PWM: freq, voice, duty_cycle(0-100)")
print("  Longo:   freq, voice, duty, start_time, duration, effect")
print("Exemplos:")
print("  Onda Quadrada 50%: 440, 0, 50")
print("  Onda Pulso 12.5%:  440, 0, 12.5")
print("  Acorde diferente:  440, 0, 50; 554.37, 1, 25")

try:
    while True:
        msg = input("\n> ").strip()
        
        if msg.lower() in ['sair', 'exit', 'quit']:
            break

        if not msg:
            continue

        comandos = [c.strip() for c in msg.split(';') if c.strip()]
        valido = True
        for c in comandos:
            is_ok, erro = validar_comando(c)
            if not is_ok:
                print(f"❌ Erro no comando '{c}': {erro}")
                valido = False
                break
        
        if valido:
            try:
                sock.sendto(msg.encode(), (ESP_IP, PORT))
                print(f"✔ Enviado: {len(comandos)} instrução(ões)")
            except Exception as e:
                print(f"❌ Erro de rede: {e}")

except KeyboardInterrupt:
    print("\n[!] Interrompido pelo usuário.")
finally:
    print("Fechando conexão...")
    sock.close()