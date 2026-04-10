import socket
import mido
import time
import os

# Configurações do ESP8266
ESP_IP = "192.168.1.7"
PORT = 8888

# --- AJUSTE DINÂMICO DE CAMINHO ---
# Isso garante que ele ache a pasta 'midis' idependente de onde você chame o terminal
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
MIDI_DIR = os.path.join(BASE_DIR, "midis")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
active_notes = {0: None, 1: None}

def note_to_freq(note):
    return 440.0 * (2.0 ** ((note - 69) / 12.0))

def send_note(freq, voice, duty=50.0):
    msg = f"{freq:.2f},{voice},{duty:.1f}"
    try:
        sock.sendto(msg.encode(), (ESP_IP, PORT))
    except:
        pass # Ignora erros de rede momentâneos para não travar o player

def play_midi(file_path):
    try:
        mid = mido.MidiFile(file_path)
        print(f"\n🎶 Tocando: {os.path.basename(file_path)}")
        print("Pressione Ctrl+C para interromper.\n")
        
        for msg in mid.play():
            if msg.type == 'note_on' and msg.velocity > 0:
                freq = note_to_freq(msg.note)
                v = 0 if active_notes[0] is None else 1
                active_notes[v] = msg.note
                duty = 5.0 + (msg.velocity / 127.0) * 45.0
                send_note(freq, v, duty)

            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                for v, note in active_notes.items():
                    if note == msg.note:
                        send_note(0, v)
                        active_notes[v] = None

    except KeyboardInterrupt:
        print("\n⏹ Música interrompida.")
    finally:
        send_note(0, 0)
        send_note(0, 1)
        time.sleep(10)
        send_note(0, 0)
        send_note(0, 1)
        time.sleep(10)
        send_note(0, 0)
        send_note(0, 1)

def selecionar_arquivo():
    # Debug para você ver onde ele está procurando
    print(f"🔍 Procurando em: {MIDI_DIR}")
    
    if not os.path.exists(MIDI_DIR):
        print(f"❌ Pasta não encontrada!")
        return None

    arquivos = [f for f in os.listdir(MIDI_DIR) if f.endswith(('.mid', '.midi'))]
    
    if not arquivos:
        print(f"⚠️  Nenhum arquivo .mid encontrado em: {MIDI_DIR}")
        return None

    print("\n--- 📂 BIBLIOTECA MIDI ---")
    for i, nome in enumerate(arquivos):
        print(f"[{i}] {nome}")
    print("[S] Sair")

    escolha = input("\nEscolha o número da música: ").strip().lower()
    
    if escolha == 's':
        return 'sair'
    
    try:
        idx = int(escolha)
        if 0 <= idx < len(arquivos):
            return os.path.join(MIDI_DIR, arquivos[idx])
    except ValueError:
        pass
    
    print("⚠️ Escolha inválida.")
    return None

if __name__ == "__main__":
    while True:
        caminho = selecionar_arquivo()
        if caminho == 'sair':
            break
        if caminho:
            play_midi(caminho)
            time.sleep(1) # Pequena pausa entre músicas