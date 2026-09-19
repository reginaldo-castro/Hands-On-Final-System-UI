#include <minui/minui.h>
#include <android-base/stringprintf.h>
#include <cutils/klog.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstdlib>

using android::base::StringPrintf;

#define LOG_TAG "handson_charger"

// Função para ler nós do sistema de forma segura
std::string readSysNode(const std::string& path) {
    std::ifstream file(path);
    std::string value = "N/A";
    if (file.is_open()) {
        std::getline(file, value);
        file.close();
    }
    return value;
}

// Função para escrever no backlight da tela
void setBacklightBrightness(int brightness) {
    const char* backlight_paths[] = {
        "/sys/class/leds/lcd-backlight/brightness",
        "/sys/class/backlight/backlight/brightness",
        "/sys/class/leds/lcd-backlight1/brightness"
    };
    
    for (const char* path : backlight_paths) {
        int fd = open(path, O_WRONLY);
        if (fd >= 0) {
            std::string brightness_str = std::to_string(brightness);
            write(fd, brightness_str.c_str(), brightness_str.length());
            close(fd);
            KLOG_INFO(LOG_TAG, "Backlight set to %d via %s", brightness, path);
            return;
        }
    }
    KLOG_WARNING(LOG_TAG, "Failed to set backlight brightness");
}

// Função para converter temperatura de milikelvin para Celsius
std::string formatTemperature(const std::string& temp_raw) {
    int temp_mk = atoi(temp_raw.c_str());
    if (temp_mk == 0 && temp_raw != "0") {
        return "N/A";
    }
    float temp_c = temp_mk / 1000.0f;
    return StringPrintf("%.1f°C", temp_c);
}

// Desenha uma barra de progresso bonita
void drawBatteryBar(int x, int y, int width, int height, int percentage) {
    // Fundo da barra (cinza escuro)
    gr_color(60, 60, 60, 255);
    gr_fill(x, y, x + width, y + height);
    
    // Borda da barra (branca)
    gr_color(255, 255, 255, 255);
    gr_fill(x - 2, y - 2, x + width + 2, y + 2);      // Topo
    gr_fill(x - 2, y + height, x + width + 2, y + height + 2);  // Base
    gr_fill(x - 2, y, x, y + height);                // Esquerda
    gr_fill(x + width, y, x + width + 2, y + height); // Direita
    
    // Barra de progresso com gradiente de cores baseado na porcentagem
    int fill_width = (width * percentage) / 100;
    if (fill_width > 0) {
        if (percentage <= 20) {
            gr_color(255, 50, 50, 255);   // Vermelho para bateria baixa
        } else if (percentage <= 50) {
            gr_color(255, 165, 50, 255);  // Laranja para bateria média
        } else {
            gr_color(50, 200, 100, 255);  // Verde para bateria boa
        }
        gr_fill(x, y, x + fill_width, y + height);
    }
}

// Desenha texto centralizado com efeito de sombra
void drawShadowText(const GRFont* font, int x, int y, const char* text, 
                    unsigned char r, unsigned char g, unsigned char b) {
    if (!font) return;
    
    // Sombra
    gr_color(0, 0, 0, 180);
    gr_text(font, x + 2, y + 2, text, false);
    
    // Texto principal
    gr_color(r, g, b, 255);
    gr_text(font, x, y, text, false);
}

int main() {
    KLOG_INFO(LOG_TAG, "=== HANDSON PREBOOT CHARGER INITIALIZING ===");
    
    // Aguarda um momento para garantir que o display esteja pronto
    sleep(1);
    
    // Ativa o backlight da tela antes da inicialização gráfica
    setBacklightBrightness(255);
    
    // Inicializa o sistema gráfico
    int gr_init_result = gr_init();
    if (gr_init_result < 0) {
        KLOG_ERROR(LOG_TAG, "Failed to initialize graphics: %d", gr_init_result);
        return 1;
    }
    KLOG_INFO(LOG_TAG, "Graphics initialized successfully");
    
    // Obtém a fonte do sistema
    const GRFont* font = gr_sys_font();
    if (!font) {
        KLOG_WARNING(LOG_TAG, "Failed to get system font, continuing without text");
    } else {
        int char_width, char_height;
        gr_font_size(font, &char_width, &char_height);
        KLOG_INFO(LOG_TAG, "Font loaded: %dx%d pixels", char_width, char_height);
    }
    
    // Obtém dimensões da tela
    int screen_width = gr_fb_width();
    int screen_height = gr_fb_height();
    KLOG_INFO(LOG_TAG, "Screen resolution: %dx%d", screen_width, screen_height);
    
    // Loop principal de renderização
    int frame_count = 0;
    while (true) {
        // Lê os nós da bateria
        std::string capacity_str = readSysNode("/sys/class/power_supply/battery/capacity");
        std::string temp_str = readSysNode("/sys/class/thermal/thermal_zone0/temp");
        std::string status_str = readSysNode("/sys/class/power_supply/battery/status");
        std::string voltage_str = readSysNode("/sys/class/power_supply/battery/voltage_now");
        
        // Converte e formata os valores
        int capacity = atoi(capacity_str.c_str());
        if (capacity < 0) capacity = 0;
        if (capacity > 100) capacity = 100;
        
        std::string temp_formatted = formatTemperature(temp_str);
        
        // Formata a tensão (convertendo de microvolts para volts)
        std::string voltage_formatted = "N/A";
        int voltage_uv = atoi(voltage_str.c_str());
        if (voltage_uv > 0 || voltage_str == "0") {
            float voltage_v = voltage_uv / 1000000.0f;
            voltage_formatted = StringPrintf("%.2fV", voltage_v);
        }
        
        // Limpa a tela com fundo preto
        gr_color(10, 10, 15, 255);  // Azul muito escuro quase preto
        gr_clear();
        
        // Calcula posições centrais
        int center_x = screen_width / 2;
        int center_y = screen_height / 2;
        
        // === CABEÇALHO ===
        int header_y = center_y - 200;
        if (font) {
            drawShadowText(font, center_x - (char_width * 12), header_y, "HANDSON", 100, 180, 255);
            drawShadowText(font, center_x - (char_width * 10), header_y + char_height + 10, "PREBOOT", 100, 180, 255);
        }
        
        // === BARRA DE BATERIA ===
        int bar_y = center_y - 50;
        int bar_width = 300;
        int bar_height = 30;
        drawBatteryBar(center_x - bar_width / 2, bar_y, bar_width, bar_height, capacity);
        
        // === PORCENTAGEM DA BATERIA ===
        std::string capacity_text = StringPrintf("%d%%", capacity);
        if (font) {
            int text_width = gr_measure(font, capacity_text.c_str());
            drawShadowText(font, center_x - text_width / 2, bar_y + bar_height + 20, 
                          capacity_text.c_str(), 255, 255, 255);
        }
        
        // === STATUS DO CARREGAMENTO ===
        int status_y = bar_y + bar_height + 60;
        std::string display_status = "CHARGING";
        if (status_str == "Full") display_status = "FULLY CHARGED";
        else if (status_str == "Discharging") display_status = "DISCHARGING";
        else if (status_str == "Not charging") display_status = "NOT CHARGING";
        
        unsigned char status_r = 100, status_g = 200, status_b = 100;
        if (status_str == "Discharging") {
            status_r = 255; status_g = 100; status_b = 100;
        } else if (status_str == "Full") {
            status_r = 100; status_g = 255; status_b = 100;
        }
        
        if (font) {
            int status_width = gr_measure(font, display_status.c_str());
            drawShadowText(font, center_x - status_width / 2, status_y, 
                          display_status.c_str(), status_r, status_g, status_b);
        }
        
        // === INFORMAÇÕES TÉCNICAS ===
        int info_y = status_y + 50;
        int info_spacing = char_height + 15;
        
        // Temperatura
        std::string temp_text = StringPrintf("SOC Temp: %s", temp_formatted.c_str());
        if (font) {
            int temp_width = gr_measure(font, temp_text.c_str());
            drawShadowText(font, center_x - temp_width / 2, info_y, 
                          temp_text.c_str(), 180, 180, 180);
        }
        
        // Tensão
        std::string voltage_text = StringPrintf("Voltage: %s", voltage_formatted.c_str());
        if (font) {
            int voltage_width = gr_measure(font, voltage_text.c_str());
            drawShadowText(font, center_x - voltage_width / 2, info_y + info_spacing, 
                          voltage_text.c_str(), 180, 180, 180);
        }
        
        // === RODAPÉ COM ANIMAÇÃO ===
        int footer_y = screen_height - 50;
        std::string footer_text = StringPrintf("Frame: %d | NIO Device", frame_count);
        if (font) {
            int footer_width = gr_measure(font, footer_text.c_str());
            drawShadowText(font, center_x - footer_width / 2, footer_y, 
                          footer_text.c_str(), 100, 100, 150);
        }
        
        // Atualiza a tela
        gr_flip();
        
        // Log a cada 10 frames para não floodar
        if (frame_count % 10 == 0) {
            KLOG_INFO(LOG_TAG, "Battery: %d%% | Status: %s | Temp: %s | Voltage: %s", 
                     capacity, status_str.c_str(), temp_formatted.c_str(), voltage_formatted.c_str());
        }
        
        frame_count++;
        usleep(500000);  // 500ms = 2 FPS para animação suave
    }
    
    // Cleanup (nunca deve chegar aqui no loop infinito, mas é boa prática)
    gr_exit();
    return 0;
}