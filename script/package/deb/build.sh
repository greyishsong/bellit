#!/usr/bin/env bash
set -euo pipefail

pkgname=bellit
pkgver=1.0.0
pkgrel=1
source_url="https://github.com/greyishsong/bellit/archive/refs/tags/v${pkgver}.tar.gz"
source_sha256="953ec45f5194cbecf3fd2fa260d5fca0982c0c5b222c3f448063fc5a92d9d3f7"

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="${script_dir}/build"
download_dir="${build_dir}/download"
work_dir="${build_dir}/work"
tarball="${download_dir}/v${pkgver}.tar.gz"
source_dir="${work_dir}/${pkgname}-${pkgver}"
orig_tarball="${work_dir}/${pkgname}_${pkgver}.orig.tar.gz"

fetch_source() {
    if [[ -f "${tarball}" ]]; then
        return
    fi

    mkdir -p "${download_dir}"
    local cached_tarball="${script_dir}/../pacman/v${pkgver}.tar.gz"
    if [[ -f "${cached_tarball}" ]]; then
        cp "${cached_tarball}" "${tarball}"
        return
    fi

    if command -v curl >/dev/null 2>&1; then
        curl --fail --location --output "${tarball}" "${source_url}"
    elif command -v wget >/dev/null 2>&1; then
        wget --output-document="${tarball}" "${source_url}"
    else
        printf 'Neither curl nor wget is available to download %s\n' "${source_url}" >&2
        exit 1
    fi
}

verify_source() {
    if ! printf '%s  %s\n' "${source_sha256}" "${tarball}" | sha256sum --check --status; then
        printf 'SHA256 mismatch for %s\n' "${tarball}" >&2
        exit 1
    fi
}

check_build_dependencies() {
    local deps_output
    if deps_output="$(cd "${source_dir}" && dpkg-checkbuilddeps 2>&1)"; then
        return
    fi

    local missing_tools=()
    for command_name in cmake dh clang git ninja; do
        if ! command -v "${command_name}" >/dev/null 2>&1; then
            missing_tools+=("${command_name}")
        fi
    done

    if ((${#missing_tools[@]} > 0)); then
        printf '%s\n' "${deps_output}" >&2
        printf 'Missing required build commands: %s\n' "${missing_tools[*]}" >&2
        exit 1
    fi

    printf '%s\n' "${deps_output}" >&2
    printf 'Continuing with dpkg-buildpackage -d because all required build commands are available.\n' >&2
    printf 'This supports builders where CMake 4.1+ is installed outside dpkg, such as /opt/cmake/bin.\n' >&2
    build_args+=(-d)
}

main() {
    export PATH="/opt/cmake/bin:${PATH}"
    local build_args=(-us -uc -b)

    fetch_source
    verify_source

    rm -rf "${source_dir}"
    mkdir -p "${work_dir}"
    tar -xf "${tarball}" -C "${work_dir}"
    cp "${tarball}" "${orig_tarball}"
    cp -a "${script_dir}/debian" "${source_dir}/debian"

    check_build_dependencies
    (cd "${source_dir}" && dpkg-buildpackage "${build_args[@]}")

    printf 'Built package artifacts:\n'
    local artifact
    for artifact in "${work_dir}"/${pkgname}_${pkgver}-${pkgrel}_*.deb; do
        if [[ -e "${artifact}" ]]; then
            printf '  %s\n' "${artifact}"
        fi
    done
}

main "$@"
