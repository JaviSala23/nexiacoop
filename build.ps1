# Script de compilación para Windows con Docker
# Asegurate de tener Docker Desktop instalado y corriendo

$ErrorActionPreference = "Stop"

Write-Host "==> Verificando Docker..." -ForegroundColor Cyan
try {
    docker --version | Out-Null
} catch {
    Write-Host "ERROR: Docker no está instalado o no está corriendo." -ForegroundColor Red
    Write-Host ""
    Write-Host "Instalá Docker Desktop desde:" -ForegroundColor Yellow
    Write-Host "https://www.docker.com/products/docker-desktop" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Después de instalar, asegurate de que Docker Desktop esté corriendo." -ForegroundColor Yellow
    exit 1
}

Write-Host ""
Write-Host "==> Construyendo imagen Docker con Nexia..." -ForegroundColor Cyan
docker build -f Dockerfile.build -t nexia-builder .

Write-Host ""
Write-Host "==> Extrayendo binario compilado..." -ForegroundColor Cyan

# Remover contenedor temporal si existe
docker rm nexia-tmp -f 2>$null

# Crear contenedor temporal
docker create --name nexia-tmp nexia-builder

# Crear directorios si no existen
if (-not (Test-Path "nexia\build")) {
    New-Item -ItemType Directory -Path "nexia\build" -Force | Out-Null
}

# Copiar binarios
docker cp nexia-tmp:/src/build/nexia nexia/build/nexia
docker cp nexia-tmp:/src/build/nexia nexia_ubuntu20

# Limpiar contenedor temporal
docker rm nexia-tmp

Write-Host ""
Write-Host "==> Empaquetando librerías del runtime..." -ForegroundColor Cyan

# Convertir bundle_runtime.sh a PowerShell o ejecutarlo con bash si está disponible
if (Get-Command bash -ErrorAction SilentlyContinue) {
    bash bundle_runtime.sh
} else {
    Write-Host "ADVERTENCIA: bash no está disponible, saltando empaquetado de runtime." -ForegroundColor Yellow
    Write-Host "Las librerías se copiarán manualmente cuando despliegues." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "✓ Listo. Binario en: nexia/build/nexia" -ForegroundColor Green
Write-Host "✓ Binario en: nexia_ubuntu20  (copia para deploy)" -ForegroundColor Green
Write-Host "✓ Libs en: runtime/" -ForegroundColor Green
