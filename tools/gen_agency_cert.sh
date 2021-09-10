#!/bin/bash
set -e

logfile=${PWD}/build.log
ca_path=
agency=
root_crt=

LOG_WARN()
{
    local content=${1}
    echo -e "\033[31m[WARN] ${content}\033[0m"
}

LOG_INFO()
{
    local content=${1}
    echo -e "\033[32m[INFO] ${content}\033[0m"
}

getname() {
    local name="$1"
    if [ -z "$name" ]; then
        return 0
    fi
    [[ "$name" =~ ^.*/$ ]] && {
        name="${name%/*}"
    }
    name="${name##*/}"
    echo "$name"
}

check_name() {
    local name="$1"
    local value="$2"
    [[ "$value" =~ ^[a-zA-Z0-9._-]+$ ]] || {
        echo "$name name [$value] invalid, it should match regex: ^[a-zA-Z0-9._-]+\$"
        exit "${EXIT_CODE}"
    }
}

file_must_exists() {
    if [ ! -f "$1" ]; then
        echo "$1 file does not exist, please check!"
        exit "${EXIT_CODE}"
    fi
}

dir_must_exists() {
    if [ ! -d "$1" ]; then
        echo "$1 DIR does not exist, please check!"
        exit "${EXIT_CODE}"
    fi
}

dir_must_not_exists() {
    if [ -e "$1" ]; then
        echo "$1 DIR exists, please clean old DIR!"
        exit "${EXIT_CODE}"
    fi
}

help()
{
    cat << EOF
Usage:
    -c <ca path>           [Required]
    -a <agency name>       [Required]
    -h Help
e.g:
    bash $0 -c nodes/cert -a newAgency
EOF
exit 0
}

parse_params()
{
    while getopts "c:a:g:h" option;do
        case $option in
        c) ca_path="${OPTARG}"
            if [ ! -f "$ca_path/ca.key" ]; then LOG_WARN "$ca_path/ca.key not exist" && exit 1; fi
            if [ ! -f "$ca_path/ca.crt" ]; then LOG_WARN "$ca_path/ca.crt not exist" && exit 1; fi
            if [ -f "$ca_path/root.crt" ]; then root_crt="$ca_path/root.crt";fi
        ;;
        a) agency="${OPTARG}"
            if [ -z "$agency" ]; then LOG_WARN "$agency not specified" && exit 1; fi
        ;;
        h) help;;
        *) LOG_WARN "invalid option $option";;
        esac
    done
}

generate_cert_conf()
{
    local output=$1
    cat << EOF > "${output}"
[ca]
default_ca=default_ca
[default_ca]
default_days = 365
default_md = sha256

[req]
distinguished_name = req_distinguished_name
req_extensions = v3_req
[req_distinguished_name]
countryName = CN
countryName_default = CN
stateOrProvinceName = State or Province Name (full name)
stateOrProvinceName_default =GuangDong
localityName = Locality Name (eg, city)
localityName_default = ShenZhen
organizationalUnitName = Organizational Unit Name (eg, section)
organizationalUnitName_default = fisco-bcos
commonName =  Organizational  commonName (eg, fisco-bcos)
commonName_default = fisco-bcos
commonName_max = 64

[ v3_req ]
basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment

[ v4_req ]
basicConstraints = CA:TRUE

EOF
}

gen_agency_cert() {
    local chain="${1}"
    local agencypath="${2}"
    name=$(basename "$agencypath")

    dir_must_exists "$chain"
    file_must_exists "$chain/ca.key"
    check_name agency "$name"
    agencydir="$agencypath"
    dir_must_not_exists "$agencydir"
    mkdir -p "$agencydir"

    # openssl genrsa -out "$agencydir/agency.key" 2048 2> /dev/null
    openssl ecparam -out "$agencydir/secp256k1.param" -name secp256k1 2> /dev/null
    openssl genpkey -paramfile "$agencydir/secp256k1.param" -out "$agencydir/agency.key" 2> /dev/null
    openssl req -new -sha256 -subj "/CN=$name/O=fisco-bcos/OU=agency" -key "$agencydir/agency.key" -out "$agencydir/agency.csr" 2> /dev/null
    openssl x509 -req -days 3650 -sha256 -CA "$chain/ca.crt" -CAkey "$chain/ca.key" -CAcreateserial\
        -in "$agencydir/agency.csr" -out "$agencydir/agency.crt"  -extensions v4_req -extfile "$chain/cert.cnf" 2> /dev/null

    cp "$chain/ca.crt" "$chain/cert.cnf" "$agencydir/"
    if [[ -n "${root_crt}" ]];then
        echo "Use user specified root cert as ca.crt, $agencydir" >>"${logfile}"
        cp "${root_crt}" "$agencydir/ca.crt"
    else
        cp "$chain/ca.crt" "$chain/cert.cnf" "$agencydir/"
    fi
    rm -f "$agencydir/agency.csr" "$agencydir/secp256k1.param"

    echo "build $name cert successful!"
}

main()
{
    if openssl version | grep -q reSSL ;then
        export PATH="/usr/local/opt/openssl/bin:$PATH"
    fi
    generate_cert_conf "${ca_path}/cert.cnf"
    gen_agency_cert "${ca_path}" "${ca_path}/${agency}" > "${logfile}" 2>&1
    rm "${logfile}"
}

print_result()
{
    echo "=============================================================="
    LOG_INFO "Cert Path   : ${ca_path}/${agency}"
    LOG_INFO "All completed."
}

parse_params "$@"
main
print_result
