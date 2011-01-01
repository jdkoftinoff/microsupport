#! /bin/bash -e

for d in tests examples tools-dev; do
  for i in ../../"${d}"/*.c ; do 
    b=$(basename ${i%.c})
    mkdir -p "${b}"
    sed "s/{{{1}}}/${b}/g;s/{{{2}}}/${d}/g" <pro >"${b}/${b}.pro"
  done
done

