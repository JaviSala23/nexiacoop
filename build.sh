#!/bin/bash
set -e

echo "==> Construyendo imagen Docker con Nexia..."
docker build -f Dockerfile.build -t nexia-builder .

echo ""
echo "==> Extrayendo binario compilado..."
docker create --name nexia-tmp nexia-builder
docker cp nexia-tmp:/src/build/nexia nexia/build/nexia
docker cp nexia-tmp:/src/build/nexia nexia_ubuntu20
docker rm nexia-tmp

echo ""
echo "==> Empaquetando librerías del runtime..."
bash bundle_runtime.sh

echo ""
echo "✓ Listo. Binario en: nexia/build/nexia"
echo "✓ Binario en: nexia_ubuntu20  (copia para deploy)"
echo "✓ Libs en: runtime/"
