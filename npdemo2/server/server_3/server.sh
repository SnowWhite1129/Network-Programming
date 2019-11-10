#! /bin/sh

SERVER_PATH=$1
SERVER_PORT=$2

if [ -z ${SERVER_PATH} ] || [ -z ${SERVER_PORT} ]; then
  echo "Usage: $0 [shell_path] [port]"
  exit 1
fi

if [ ! -x ${SERVER_PATH} ] ; then
  echo "There is something wrong with your server."
  echo "Please make sure it exists and it has the correct permission."
  exit 1
fi


SERVER_PATH=$( readlink -f ${SERVER_PATH} )

[ -n "$( which gmake )" ] && MAKE=gmake
[ -z "$( which gmake )" ] && MAKE=make
${MAKE}

cd work_dir
echo "[1;34m======= Your server is running =======[m"
${SERVER_PATH} ${SERVER_PORT}
echo "[1;34m======= Your server terminated =======[m"
