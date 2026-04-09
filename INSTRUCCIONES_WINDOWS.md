# Compilar Nexia en Windows

## 1. Instalar Docker Desktop

1. Descargá Docker Desktop desde: https://www.docker.com/products/docker-desktop
2. Instalalo (requiere reiniciar Windows)
3. Abrí Docker Desktop y esperá a que esté completamente iniciado (el ícono en la bandeja debería estar verde)

## 2. Verificar que Docker funciona

Abrí PowerShell y ejecutá:

```powershell
docker --version
```

Deberías ver algo como: `Docker version 24.x.x, build...`

## 3. Compilar el proyecto

Desde la carpeta del proyecto, ejecutá:

```powershell
.\build.ps1
```

Esto va a:
- Construir una imagen Docker con Ubuntu 20.04
- Compilar Drogon y todas las dependencias
- Compilar Nexia
- Extraer el binario compilado a `nexia/build/nexia` y `nexia_ubuntu20`

## 4. Si tenés problemas con Docker

Si Docker no inicia o da error de "WSL 2", necesitás:

1. Habilitar WSL 2:
   ```powershell
   wsl --install
   ```

2. Reiniciar Windows

3. Volver a abrir Docker Desktop

## 5. Compilación manual (si no querés usar el script)

También podés ejecutar los comandos manualmente:

```powershell
# Construir imagen
docker build -f Dockerfile.build -t nexia-builder .

# Crear contenedor temporal
docker create --name nexia-tmp nexia-builder

# Copiar binario
docker cp nexia-tmp:/src/build/nexia nexia/build/nexia
docker cp nexia-tmp:/src/build/nexia nexia_ubuntu20

# Limpiar
docker rm nexia-tmp
```

## 6. Después de compilar

El binario `nexia_ubuntu20` es para deployar en el servidor Linux.
Copialo junto con las carpetas `runtime/` y `views/` al servidor.
