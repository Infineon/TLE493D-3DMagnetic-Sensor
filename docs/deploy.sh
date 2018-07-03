# TODO: change to official url
REPO_NAME=TLE493D-W2B6-3DMagnetic-Sensor
USR_NAME="Sun Yuxi (IFAG DES SDF FW)"
PRIVATE_NAME=sherylll
USR_EMAIL="Yuxi.Sun@infineon.com"
GH_REPO_REF=github.com/$PRIVATE_NAME/$REPO_NAME.git

sudo apt-get install graphviz
#install texlive (instead of miktex)
sudo apt-get -qq update && sudo apt-get install -y --no-install-recommends texlive-full
#install doxygen 1.8.11
set -ex
DOXYGEN_VER=doxygen-1.8.11
DOXYGEN_TAR=${DOXYGEN_VER}.linux.bin.tar.gz
DOXYGEN_URL="http://ftp.stack.nl/pub/users/dimitri/${DOXYGEN_TAR}"
wget -O - "${DOXYGEN_URL}" | tar xz -C ${TMPDIR-/tmp} ${DOXYGEN_VER}/bin/doxygen
sudo mv ${TMPDIR-/tmp}/${DOXYGEN_VER}/bin/doxygen /usr/bin/

make 
  
# if file exists
if [ -f "library.pdf" ]; then  
  git clone -b docs https://git@$GH_REPO_REF
  cd $REPO_NAME
  git config --global push.default simple
  git config user.name $USR_NAME
  git config user.email $USR_EMAIL

  rm -rf *
  cp ../library.pdf .
  git add library.pdf
  git commit -m "doc update by commit ${TRAVIS_COMMIT}"
  
  # The ouput is redirected to /dev/null to hide any sensitive credential data
  # that might otherwise be exposed.
  git push "https://${GH_REPO_TOKEN}@${GH_REPO_REF}" > /dev/null 2>&1
fi