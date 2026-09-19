# Git Guide: Push handson_charger sem enviar AOSP completo

## 📋 Visão Geral

Este guia explica como fazer push das modificações do `handson_charger` para o Git sem enviar todo o AOSP (Android Open Source Project), que é extremamente grande.

## 🎯 Objetivo

- Criar branch específica para handson_charger
- Commitar apenas arquivos modificados do projeto
- Evitar enviar arquivos grandes do AOSP
- Push limpo e organizado

## 📁 Estrutura de Arquivos Modificados

Os arquivos que queremos versionar são:

```
vendor/handson/charger_custom/
├── Android.bp
├── main.cpp
└── init.handson_charger.rc

device/motorola/nio/
├── device.mk
└── lineage_nio.mk

README_HANDSON_CHARGER.md
```

## 🔧 Configuração Inicial

### 1. Verificar Status do Git

```bash
cd /home/devtitans-1/android
git status
```

### 2. Criar .gitignore (se não existir)

```bash
cd /home/devtitans-1/android
cat > .gitignore << 'EOF'
# AOSP build outputs
out/
*.o
*.a
*.so
*.lo
*.pyc
*.class

# AOSP source tree (opcional - comente se quiser versionar AOSP)
# bootable/
# build/
# cts/
# development/
# device/
# external/
# frameworks/
# hardware/
# kernel/
# packages/
# prebuilts/
# system/
# tools/
# vendor/

# IDE
.idea/
.vscode/
*.swp
*.swo

# OS
.DS_Store
Thumbs.db
EOF
```

### 3. Criar Branch Específica

```bash
# Criar nova branch para handson_charger
git checkout -b feature/handson-charger

# Ou para branch específica com nome customizado
git checkout -b handson-charger-offline-charging
```

## 📝 Procedimento de Commit

### Passo 1: Adicionar Arquivos Específicos

```bash
cd /home/devtitans-1/android

# Adicionar apenas arquivos do handson_charger
git add vendor/handson/charger_custom/Android.bp
git add vendor/handson/charger_custom/main.cpp
git add vendor/handson/charger_custom/init.handson_charger.rc

# Adicionar modificações de configuração do dispositivo
git add device/motorola/nio/device.mk
git add device/motorola/nio/lineage_nio.mk

# Adicionar documentação
git add README_HANDSON_CHARGER.md
```

### Passo 2: Verificar Staging Area

```bash
# Verificar arquivos que serão commitados
git status

# Verificar diff antes do commit
git diff --cached
```

### Passo 3: Criar Commit

```bash
# Commit descritivo
git commit -m "feat: add custom offline charging screen

- Implement handson_charger with libminui integration
- Add init service for charger mode
- Update device configuration to include handson_charger
- Add comprehensive documentation

Generated with [Devin](https://devin.ai)

Co-Authored-By: Devin <158243242+devin-ai-integration[bot]@users.noreply.github.com>"
```

### Passo 4: Verificar Commit

```bash
# Verificar últimos commits
git log --oneline -5

# Verificar detalhes do último commit
git show HEAD
```

## 🚀 Push para Repositório Remoto

### Passo 1: Configurar Remote (se necessário)

```bash
# Verificar remotes existentes
git remote -v

# Adicionar remote GitHub/GitLab (se não existir)
git remote add origin https://github.com/SEU_USUARIO/SEU_REPOSITORIO.git

# Ou para SSH (recomendado)
git remote add origin git@github.com:SEU_USUARIO/SEU_REPOSITORIO.git
```

### Passo 2: Push da Branch

```bash
# Push da branch específica
git push -u origin feature/handson-charger

# Ou com nome customizado
git push -u origin handson-charger-offline-charging
```

### Passo 3: Verificar no Remote

```bash
# Verificar branches remotas
git branch -r

# Verificar status da branch
git status
```

## 🔄 Procedimento Atualização (Futuras Modificações)

### Quando fizer novas alterações:

```bash
cd /home/devtitans-1/android

# 1. Fazer alterações nos arquivos
# ... editar arquivos ...

# 2. Verificar mudanças
git status

# 3. Adicionar apenas arquivos modificados
git add vendor/handson/charger_custom/
git add device/motorola/nio/
git add README_HANDSON_CHARGER.md

# 4. Commit
git commit -m "fix: update handson_charger implementation"

# 5. Push
git push
```

## 🛡️ Boas Práticas

### 1. Usar Git Sparse Checkout (Para Repositórios AOSP)

Se você clonou todo o AOSP e quer trabalhar apenas com partes específicas:

```bash
# Clonar com sparse checkout
git clone --no-checkout https://github.com/SEU_USUARIO/SEU_REPOSITORIO.git
cd SEU_REPOSITORIO
git sparse-checkout init --cone
git sparse-checkout set vendor/handson/charger_custom device/motorola/nio
git checkout
```

### 2. Usar Git LFS para Arquivos Grandes

Se precisar versionar arquivos grandes:

```bash
# Instalar git-lfs
sudo apt install git-lfs

# Inicializar no repositório
git lfs install

# Rastrear arquivos grandes
git lfs track "*.img"
git lfs track "*.zip"
git add .gitattributes
```

### 3. Revisar Commits Antes de Push

```bash
# Rebase interativo para modificar commits
git rebase -i HEAD~3

# Ou resetar último commit se necessário
git reset --soft HEAD~1
```

## 📊 Comandos Úteis

### Verificação

```bash
# Ver histórico de commits
git log --graph --oneline --all

# Ver arquivos modificados em commits
git log --name-only --oneline

# Ver tamanho do repositório
du -sh .git
```

### Limpeza

```bash
# Limpar arquivos não rastreados
git clean -fd

# Resetar para estado anterior
git reset --hard HEAD

# Remover arquivo do staging
git reset HEAD caminho/arquivo
```

## 🚨 Solução de Problemas

### Erro: "Repository is too large"

```bash
# Usar git gc para limpar
git gc --aggressive --prune=now

# Ou usar shallow clone
git clone --depth 1 https://github.com/SEU_USUARIO/SEU_REPOSITORIO.git
```

### Erro: "File too large"

```bash
# Configurar tamanho máximo
git config http.postBuffer 524288000

# Ou usar git lfs
git lfs track "arquivo_grande"
git add arquivo_grave
git commit -m "Add large file with LFS"
```

### Push Rejeitado

```bash
# Fazer pull antes de push
git pull origin feature/handson-charger --rebase

# Depois push
git push origin feature/handson-charger
```

## 📝 Fluxo de Trabalho Recomendado

```bash
# 1. Atualizar branch principal
git checkout main
git pull origin main

# 2. Criar branch de feature
git checkout -b feature/handson-charger-update

# 3. Fazer modificações
# ... editar arquivos ...

# 4. Commitar apenas arquivos modificados
git add vendor/handson/charger_custom/
git add device/motorola/nio/
git commit -m "feat: implement new feature"

# 5. Push
git push -u origin feature/handson-charger-update

# 6. Criar Pull Request no GitHub/GitLab
```

## 🎯 Resumo Rápido

```bash
# Setup inicial (única vez)
cd /home/devtitans-1/android
git checkout -b feature/handson-charger
git remote add origin git@github.com:SEU_USUARIO/SEU_REPOSITORIO.git

# Commit inicial
git add vendor/handson/charger_custom/ device/motorola/nio/ README_HANDSON_CHARGER.md
git commit -m "feat: add handson_charger offline charging"
git push -u origin feature/handson-charger

# Atualizações futuras
git add vendor/handson/charger_custom/ device/motorola/nio/ README_HANDSON_CHARGER.md
git commit -m "fix: update implementation"
git push
```

## 📚 Referências

- [Git Documentation](https://git-scm.com/doc)
- [GitHub Sparse Checkout](https://docs.github.com/en/repositories/working-with-files/using-files/checking-out-files)
- [Git LFS](https://git-lfs.github.com/)
