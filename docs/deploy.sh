# TODO: change to official url
REPO_NAME=TLE493D-W2B6-3DMagnetic-Sensor
USR_NAME="Sun Yuxi (IFAG DES SDF FW)"
PRIVATE_NAME=sherylll
USR_EMAIL="Yuxi.Sun@infineon.com"
GH_REPO_REF=github.com/$PRIVATE_NAME/$REPO_NAME.git

# if file exists
if [ -f "library.pdf" ]; then
  sudo apt-get install doxygen graphviz
  #install miktex
  sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys D6BC243565B2087BC3F897C9277A7293F59E4889
  echo "deb http://miktex.org/download/ubuntu xenial universe" | sudo tee /etc/apt/sources.list.d/miktex.list
  sudo apt-get update
  sudo apt-get install miktex
  miktexsetup finish
  
  git clone -b docs https://git@$GH_REPO_REF
  cd REPO_NAME
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