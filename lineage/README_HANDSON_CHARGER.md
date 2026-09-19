# Handson Preboot Charger - Custom Offline Charging Screen

## 📋 Descrição do Projeto

Este projeto substitui a animação padrão de bateria do Android (Offline Charging) por uma interface gráfica customizada desenvolvida em C++ que utiliza a biblioteca `libminui` do Android Recovery. O binário é instalado em `/vendor/bin/handson_charger` e executa quando o dispositivo está desligado conectado ao carregador.

### 🎯 Dispositivo Alvo
- **Modelo**: Motorola Moto G100
- **Codinome**: `nio`
- **Sistema**: LineageOS 21.0 (Android 14)
- **Arquitetura**: ARM64 (armv8-a)

---

## 🔧 Modificações Implementadas

### 1. Atualização do Android.bp
**Arquivo**: `Android.bp`

**Alterações**:
- Adicionada dependência `libminui` - Biblioteca gráfica nativa do Android Recovery/Charger
- Adicionada dependência `libpng` - Processamento de imagens e recursos gráficos
- Adicionada dependência `libz` - Biblioteca de compressão necessária para libminui
- Mantidas as dependências originais: `libbase`, `libcutils`, `libutils`

**Objetivo**: Habilitar renderização gráfica direta no framebuffer do dispositivo sem depender do Android Runtime.

### 2. Reescrita Completa do main.cpp
**Arquivo**: `main.cpp`

#### Funcionalidades Implementadas:

**🎨 Interface Gráfica:**
- **Inicialização do sistema gráfico** via `gr_init()` com verificação de erros
- **Carregamento de fontes do sistema** usando `gr_sys_font()`
- **Controle de backlight** com fallback para múltiplos caminhos de hardware
- **Renderização em loop** a 2 FPS (500ms) para performance otimizada

**📊 Elementos Visuais:**
- **Cabeçalho "HANDSON PREBOOT"** com cor azul vibrante (RGB: 100, 180, 255)
- **Barra de bateria animada** com gradiente de cores dinâmico:
  - 🔴 Vermelho (≤20% - bateria crítica)
  - 🟠 Laranja (21-50% - bateria média)
  - 🟢 Verde (>50% - bateria boa)
- **Porcentagem da bateria** em destaque branco
- **Status de carregamento** com cores dinâmicas baseadas no estado:
  - Verde: Carregando
  - Verde brilhante: Completamente carregada
  - Vermelho: Descarregando
  - Cinza: Não carregando
- **Informações técnicas**:
  - Temperatura do SOC (convertida de milikelvin para Celsius)
  - Tensão da bateria (convertida de microvolts para volts)
- **Rodapé animado** com contador de frames e identificação do dispositivo

**⚙️ Funcionalidades Técnicas:**
- **Leitura de nós do kernel**:
  - `/sys/class/power_supply/battery/capacity` - Capacidade da bateria
  - `/sys/class/thermal/thermal_zone0/temp` - Temperatura do SOC
  - `/sys/class/power_supply/battery/status` - Status do carregamento
  - `/sys/class/power_supply/battery/voltage_now` - Tensão atual
- **Controle de backlight** com fallback para múltiplos caminhos:
  - `/sys/class/leds/lcd-backlight/brightness`
  - `/sys/class/backlight/backlight/brightness`
  - `/sys/class/leds/lcd-backlight1/brightness`
- **Logging via Android Logging System** (`ALOGI`, `ALOGE`) para debug via logcat/dmesg
- **Tratamento robusto de erros** com try-catch para conversões de dados
- **Centralização de cálculos** de posicionamento para responsividade

**🔧 Características de Código Senior:**
- Uso de `android::base::StringPrintf` para formatação segura de strings
- Funções auxiliares modulares e reutilizáveis (`readSysNode`, `setBacklightBrightness`, `formatTemperature`, `drawBatteryBar`, `drawShadowText`)
- Tratamento de edge cases (temperatura inválida, tensão ausente, capacidade fora de range)
- Código otimizado para performance em hardware limitado
- Limpeza adequada de recursos com `gr_exit()`

---

## 🚀 Procedimento Padrão de Execução

Siga estes passos sequencialmente toda vez que precisar compilar e testar o projeto no dispositivo.

### Pré-requisitos
- Ambiente de build do Android configurado
- Dispositivo Moto G100 (nio) conectado via ADB
- ADB debugging habilitado no dispositivo
- Permissões de root no dispositivo

### Passo 1: Configurar Ambiente de Build
```bash
cd /home/devtitans-1/android/lineage
source build/envsetup.sh
export TARGET_RELEASE=ap2a
lunch lineage_nio-ap2a-userdebug
```

### Passo 2: Compilar o Módulo handson_charger
```bash
mmm vendor/handson/charger_custom
```c

**Verificação de Sucesso**:
- O binário será gerado em: `out/target/product/nio/vendor/bin/handson_charger`
- Não deve haver erros de compilação

### Passo 3: (Opcional) Compilar ROM Completa
Se desejar incluir o módulo na imagem da ROM:
```bash
make -j$(nproc)
```

### Passo 4: Transferir Binário e Bibliotecas para Dispositivo
```bash
adb push out/target/product/nio/vendor/bin/handson_charger /vendor/bin/
adb push out/target/product/nio/vendor/lib64/libminui.so /vendor/lib64/
adb push out/target/product/nio/vendor/lib64/libpng.so /vendor/lib64/
```

### Passo 5: Configurar Permissões e SELinux
```bash
adb root
adb shell setenforce 0  # Modo permissive SELinux
adb shell chmod 755 /vendor/bin/handson_charger
```

### Passo 6: Testar handson_charger (com Android ativo)
```bash
adb shell /vendor/bin/handson_charger &
```

**Nota**: O comando anterior `adb shell stop` foi removido porque o display fica inacessível quando o Android é completamente parado. O handson_charger funciona corretamente quando executado em background com o Android ativo.

### Passo 7: (Opcional) Habilitar modo offline charging completo
Para habilitar o verdadeiro offline charging (quando o dispositivo está desligado conectado ao carregador), siga estes passos completos:

#### 7.1 Recompilar a ROM completa
```bash
make -j$(nproc)
```

#### 7.2 Preparar o dispositivo para flash

**Habilitar OEM Unlock e USB Debugging (se ainda não estiver):**
- No dispositivo: Configurações > Sobre o telefone > Toque 7x em "Número da versão"
- Configurações > Opções do desenvolvedor > OEM Unlock (ativar)
- Configurações > Opções do desenvolvedor > USB Debugging (ativar)

**Desbloquear o Bootloader (se ainda não estiver desbloqueado):**
```bash
adb reboot bootloader
fastboot oem get_unlock_data
# Copie o código e obtenha a chave de desbloqueio no site da Motorola
fastboot oem unlock [chave-obtida]
# Confirme no dispositivo
```

#### 7.3 Flashar a Recovery do LineageOS
```bash
cd /home/devtitans-1/android/lineage
fastboot flash boot out/target/product/nio/boot.img
```

#### 7.4 Entrar no Recovery do LineageOS
```bash
fastboot reboot recovery
# Ou no bootloader: Volume Down para selecionar "Recovery", Power para confirmar
```

#### 7.5 Limpar Dados (Factory Reset)
No recovery do LineageOS:
- Selecione "Factory Reset" ou "Wipe Data/Factory Reset"
- Confirme "Format Data"
- **Importante**: Isso é necessário para evitar conflitos de chaves de criptografia

#### 7.6 Fazer Sideload da ROM
No recovery:
- Selecione "Apply Update" > "Apply from ADB"
- No seu computador:
```bash
adb sideload out/target/product/nio/lineage-*.zip
```

#### 7.7 Reiniciar o Dispositivo
- Após o sideload completar, selecione "Reboot system now"
- O primeiro boot pode demorar 5-10 minutos

#### 7.8 Verificar a Instalação
```bash
# Verificar versão da ROM
adb shell getprop ro.lineage.version
adb shell getprop ro.build.display

# Verificar handson_charger instalado
adb shell ls -la /vendor/bin/handson_charger
adb shell ls -la /vendor/etc/init/init.handson_charger.rc
```

#### 7.9 Testar Offline Charging

**Teste inicial (com Android ativo):**
```bash
adb root
adb shell setenforce 0
adb shell /vendor/bin/handson_charger &
```

**Teste verdadeiro offline charging:**
1. Desligue o dispositivo completamente
2. Conecte o carregador
3. O handson_charger deve iniciar automaticamente e mostrar a interface customizada

**Nota**: A configuração init está em `init.handson_charger.rc` e é ativada automaticamente quando o dispositivo entra em modo charger.

### Passo 8: Monitorar Logs
Em um terminal separado, monitore os logs:

**Via logcat**:
```bash
adb logcat | grep -i handson
```

**Via dmesg**:
```bash
adb shell dmesg | grep -i handson
```

### Passo 9: Verificar Visualização
- A tela do dispositivo deve mostrar a interface gráfica customizada sobre a interface Android
- Deve exibir: logo HANDSON, barra de bateria, porcentagem, status, temperatura e tensão
- A interface deve atualizar a cada 500ms (2 FPS)
- **Nota**: Como está rodando sobre o Android ativo, a interface pode aparecer sobreposta à tela existente

### Passo 10: Encerrar Teste
Para parar o handson_charger:
```bash
adb shell pkill handson_charger
```

---

## � Guia Completo de Flash da ROM

### Pré-requisitos
- Dispositivo Moto G100 (nio) com bootloader desbloqueado
- Cabo USB
- Bateria com pelo menos 50% de carga
- Backup dos dados importantes (recomendado)

### Passo 1: Preparar o Dispositivo

**Habilitar Modo Desenvolvedor:**
1. Configurações > Sobre o telefone
2. Toque 7 vezes em "Número da versão"
3. "Você agora é um desenvolvedor" aparecerá

**Habilitar Opções de Desenvolvedor:**
1. Configurações > Sistema > Opções do desenvolvedor
2. Ativar "OEM desbloqueio"
3. Ativar "Depuração USB"

**Desbloquear Bootloader (se necessário):**
```bash
adb reboot bootloader
fastboot oem get_unlock_data
# Copie o código (16 caracteres)
# Acesse https://motorola-global-en-gm.custhelp.com/app/standalone-bootloader-unlock
# Cole o código e solicite a chave de desbloqueio
fastboot oem unlock [sua-chave-aqui]
# Confirme no dispositivo com Volume Up + Power
```

### Passo 2: Compilar a ROM (se ainda não compilou)
```bash
cd /home/devtitans-1/android/lineage
source build/envsetup.sh
export TARGET_RELEASE=ap2a
lunch lineage_nio-ap2a-userdebug
make -j$(nproc)
```

### Passo 3: Flashar Recovery do LineageOS
```bash
# Reiniciar para bootloader
adb reboot bootloader

# Flashar boot.img (contém recovery)
fastboot flash boot out/target/product/nio/boot.img

# Reiniciar para recovery
fastboot reboot recovery
```

### Passo 4: Factory Reset no Recovery
No recovery do LineageOS:
1. Use Volume Up/Down para navegar
2. Selecione "Factory Reset" ou "Wipe Data/Factory Reset"
3. Selecione "Format data" 
4. Digite "yes" para confirmar
5. **Importante**: Isso apaga todos os dados do dispositivo

### Passo 5: Sideload da ROM
No recovery:
1. Selecione "Apply Update" > "Apply from ADB"
2. No computador:
```bash
adb sideload out/target/product/nio/lineage-*.zip
```
3. Aguarde o processo completar (pode demorar vários minutos)

### Passo 6: Reiniciar e Configurar
1. No recovery, selecione "Reboot system now"
2. Aguarde o primeiro boot (5-10 minutos)
3. Configure o dispositivo normalmente

### Passo 7: Verificar Instalação
```bash
# Verificar versão
adb shell getprop ro.lineage.version
adb shell getprop ro.build.display

# Verificar handson_charger
adb shell ls -la /vendor/bin/handson_charger
adb shell ls -la /vendor/etc/init/init.handson_charger.rc
```

### Passo 8: Testar handson_charger
```bash
# Teste com Android ativo
adb root
adb shell setenforce 0
adb shell /vendor/bin/handson_charger &
```

### Passo 9: Testar Offline Charging Real
1. Desligue o dispositivo completamente
2. Conecte o carregador
3. A interface handson_charger deve aparecer automaticamente

### Troubleshooting de Flash

**Dispositivo não entra em modo bootloader:**
- Mantenha Volume Down + Power pressionados ao ligar
- Ou use: `adb reboot bootloader`

**Erro "device unauthorized":**
- Revogue autorizações USB debugging
- Reative USB debugging no dispositivo
- Acepte a conexão no dispositivo

**Erro "lockscreen disabled":**
- Bootloader pode estar bloqueado
- Siga o processo de desbloqueio do bootloader

**Bootloop após flash:**
- Entre em recovery novamente
- Faça Factory Reset
- Tente sideload novamente

**SIDeload falha:**
- Verifique a conexão USB
- Tente outro cabo USB
- Reinicie o recovery e tente novamente

---

## �🐛 Troubleshooting

### Tela continua preta após execução
**Causa possível**: Backlight não ativado
**Solução**: Verifique os logs para mensagens sobre backlight. Experimente outros caminhos de backlight no código.

### Erro "Failed to initialize graphics"
**Causa possível**: Problema com DRM/FBDEV backend
**Solução**: Verifique se o dispositivo suporta os backends gráficos. Consulte logs do kernel para erros de DRM.

### Erro de permissão "Permission denied"
**Causa possível**: SELinux bloqueando execução
**Solução**: 
```bash
adb shell setenforce 0
adb shell chmod 755 /vendor/bin/handson_charger
```

### Valores de bateria mostram "N/A"
**Causa possível**: Nós do kernel não acessíveis
**Solução**: Verifique se os caminhos dos nós estão corretos para seu dispositivo:
```bash
adb shell ls /sys/class/power_supply/battery/
adb shell cat /sys/class/power_supply/battery/capacity
```

### Erro de compilação "undefined reference to gr_init"
**Causa possível**: libminui não linkada
**Solução**: Verifique se `libminui` está no `shared_libs` do Android.bp

### Erro "CANNOT LINK EXECUTABLE: library libminui.so not found"
**Causa possível**: Bibliotecas dependente não copiadas
**Solução**: Copie as bibliotecas necessárias:
```bash
adb push out/target/product/nio/vendor/lib64/libminui.so /vendor/lib64/
adb push out/target/product/nio/vendor/lib64/libpng.so /vendor/lib64/
```

### Erro "cannot open any framebuffer: No such file or directory"
**Causa possível**: Display inacessível quando Android está completamente parado
**Solução**: Execute com Android ativo em background:
```bash
adb shell start  # Reinicie o Android se necessário
adb shell /vendor/bin/handson_charger &
```

---

## 📁 Estrutura de Arquivos

```
vendor/handson/charger_custom/
├── Android.bp                 # Configuração de build do módulo
├── main.cpp                   # Código fonte C++ principal
├── init.handson_charger.rc    # Configuração init do Android para offline charging
└── README.md                  # Este arquivo de documentação
```

### Integração com Sistema
- **device.mk**: Inclui `init.handson_charger.rc` nos pacotes do dispositivo
- **init.handson_charger.rc**: Configura o serviço para iniciar automaticamente em modo charger

---

## 🔮 Recursos Utilizados

### Bibliotecas do Android
- **libminui**: Biblioteca gráfica nativa do Android Recovery
- **libbase**: Utilitários base do Android (StringPrintf, logging)
- **libcutils**: Utilitários C do Android
- **libutils**: Framework utils do Android
- **libpng**: Processamento de imagens PNG
- **libz**: Compressão/descompressão

### Headers do Android
- `<minui/minui.h>`: Funções gráficas (gr_init, gr_text, gr_flip, etc.)
- `<android-base/logging.h>`: Sistema de logging (ALOGI, ALOGE)
- `<android-base/stringprintf.h>`: Formatação de strings segura

### Sysfs Nodes Utilizados
- `/sys/class/power_supply/battery/*`: Informações da bateria
- `/sys/class/thermal/thermal_zone0/temp`: Temperatura do SOC
- `/sys/class/leds/lcd-backlight/brightness`: Controle de backlight

---

## 📝 Notas de Desenvolvimento

### Performance
- Loop de renderização otimizado para 2 FPS (500ms)
- Uso mínimo de CPU para economizar bateria durante carregamento
- Buffer gráfico reutilizado entre frames

### Compatibilidade
- Código projetado para Android 14 (API 34)
- Testado no Moto G100 (nio) com chipset Snapdragon 870
- Deve ser compatível com dispositivos que suportam libminui

### Segurança
- Tratamento de erros para evitar crashes
- Validação de dados lidos do sysfs
- Limpeza adequada de recursos

---

## 🤝 Contribuição

Este projeto é parte da customização Handson OS para LineageOS. Modificações devem manter compatibilidade com o sistema de build do Android e seguir as convenções de código do projeto.

---

## 📄 Licença

Este código segue a mesma licença do projeto LineageOS (Apache 2.0) e é distribuído como parte da customização Handson OS.

---

**Última Atualização**: 2026-09-18  
**Versão**: 1.0  
**Autor**: Handson Development Team