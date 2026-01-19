for dir in */
do
    echo "compiling from: $dir"
    for file in ${dir}*.frag
    do 
        echo "compiling: $file"
        outp=${file##*/}
        outp=${outp%%.*}
        glslc $file -o ../spvs/${outp}_frag.spv
    done
    for file in ${dir}*.vert
    do 
        echo "compiling: $file"
        outp=${file##*/}
        outp=${outp%%.*}
        glslc $file -o ../spvs/${outp}_vert.spv
    done
done