#!/bin/bash
set -e

echo "==> Construyendo imagen Docker con Nexia..."
docker build -f Dockerfile.build -t nexia-builder .

echo ""
echo "==> Extrayendo binario y librerías del contenedor Ubuntu 20.04..."
docker rm -f nexia-tmp 2>/dev/null || true
docker create --name nexia-tmp nexia-builder
docker cp nexia-tmp:/src/build/nexia nexia/build/nexia
docker cp nexia-tmp:/src/build/nexia nexia_ubuntu20

# Extraer libs desde DENTRO del contenedor (Ubuntu 20.04, no el host)
rm -rf runtime && mkdir -p runtime
docker run --rm -v "$PWD/runtime:/out" nexia-builder bash -c '
  BINARY=/src/build/nexia
  ldd "$BINARY" | awk "{print \$3}" | grep "^/" | while read lib; do
    name=$(basename "$lib")
    # Saltar glibc, loader, pthread, libc base
    echo "$name" | grep -qE "^(libc\.|libm\.|libpthread|libdl\.|librt\.|libresolv\.|ld-linux)" && continue
    cp -L "$lib" "/out/$name" && echo "  [OK] $name"
  done
'

docker rm nexia-tmp

echo ""
echo "✓ Listo. Binario en: nexia/build/nexia"
echo "✓ Binario en: nexia_ubuntu20  (copia para deploy)"
echo "✓ Libs en: runtime/"
