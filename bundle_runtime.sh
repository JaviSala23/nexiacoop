#!/bin/bash
# bundle_runtime.sh
# Copia todas las librerías del binario nexia a la carpeta runtime/
# Ejecutar desde la PC de compilación, antes de subir al servidor.

set -e

BINARY="nexia/build/nexia"
RUNTIME_DIR="runtime"

if [ ! -f "$BINARY" ]; then
    echo "ERROR: No se encontró el binario en $BINARY"
    echo "Compilá primero con: cd nexia/build && make -j\$(nproc)"
    exit 1
fi

mkdir -p "$RUNTIME_DIR"

echo "Recolectando librerías de: $BINARY"
echo ""

# ldd devuelve líneas como:
#   libssl.so.3 => /usr/lib/x86_64-linux-gnu/libssl.so.3 (0x...)
#   linux-vdso.so.1 => (virtual, skip)
#   /lib64/ld-linux-x86-64.so.2 (el loader, skip)

COPIED=0
SKIPPED=0

while IFS= read -r line; do
    # Extraer la ruta real (el "=> /path/to/lib")
    path=$(echo "$line" | awk '{print $3}' | grep '^/')
    if [ -z "$path" ]; then
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    # Saltar el loader del sistema (ld-linux) — siempre debe ser el del server
    if echo "$path" | grep -q 'ld-linux'; then
        echo "  [SKIP]  $path  (loader del sistema)"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    # Saltar libpthread, libc, libm — son parte del glibc del server
    libname=$(basename "$path")
    if echo "$libname" | grep -qE '^(libc|libm|libpthread|libdl|librt|libresolv|libnss)'; then
        echo "  [SKIP]  $path  (glibc — siempre del server)"
        SKIPPED=$((SKIPPED + 1))
        continue
    fi

    if cp -L "$path" "$RUNTIME_DIR/$libname" 2>/dev/null; then
        echo "  [OK]    $libname"
        COPIED=$((COPIED + 1))
    else
        echo "  [WARN]  No se pudo copiar: $path"
    fi

done < <(ldd "$BINARY")

echo ""
echo "Listo: $COPIED librerías copiadas en ./$RUNTIME_DIR/  ($SKIPPED saltadas)"
echo ""
echo "Próximos pasos:"
echo "  1. scp -r $RUNTIME_DIR root@mantis:/home/Nexia/"
echo "  2. scp $BINARY root@mantis:/home/Nexia/nexia/nexia"
echo "  3. scp config.json root@mantis:/home/Nexia/nexia/"
echo "  4. scp nexia.service root@mantis:/etc/systemd/system/"
echo "  5. En el server:"
echo "       systemctl daemon-reload"
echo "       systemctl enable nexia"
echo "       systemctl start nexia"
echo "       systemctl status nexia"
