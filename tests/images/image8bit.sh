#!/bin/bash
#https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=15903
#   echo "P2 4 1   3    0 1 2 3" | convert - -scale x200 show: # for BNW images

inputImg="input.png"
outImg="output.png"

# perform some arbitrary math on the histogram -- can be whatever algorithm you desire
# in this example, just add pi to each histogram value
adjustedHist=(`convert $inputImg -format "%c" histogram:info:- |\
	tr -cs '0-9\012' ' ' |\
	awk '{ bin[$2] = $1; arrLen=length(bin) }
		END { for (i=0;i<65535;i++) 
				{newVal = bin[i] + 3.14159265; print newVal; } 
			} '`)
				
# find normalized cumulative sum of new histogram
cumSum=(`echo ${adjustedHist[*]} |
	awk ' # first find the sum of histogram counts
	func sum(s){split(s,a);r=0;for(i in a)r+=a[i];return r}{val=sum($0)}	
	END {split($0,inputHist);arrLen=length(inputHist)} 
	END {for (i=1;i<=arrLen;i++) {cumsum += inputHist[i] / val; print cumsum; } } '`) 

# find max pixel value in original image
maxPixel=(`convert $inputImg -format "%[max]" info:-`)

# create remapping function betweeen 0 and max pixel value
remapFun=(`echo ${cumSum[*]} |
	awk -v maxPixel="$maxPixel" '{split($0,inputHist);arrLen=length(inputHist)}
	END {for (i=0;i<arrLen;i++) {remap = int(inputHist[i]*maxPixel); print remap; } } '`) 
	
# rescale remapping function to 256 bins - rough, no interpolation
remapBins=${#remapFun[*]}
spacedRemap=(`echo ${remapFun[*]} | 
		awk -v remapBins="$remapBins" '{split($0,inputHist); binSpace=int(remapBins/256)}
		END {for (i=0;i<=256;i++) {scaleRemap = inputHist[i*binSpace]; print scaleRemap; }}'`)
newBins=${#spacedRemap[*]}

# now convert remapFun into look up table (lut) image
# Use NetPBM (PGM format implied intermediate image)
echo "P2 $newBins 1 65535 ${spacedRemap[*]}" | convert - remap.png

# now apply the lut and save image
# need to multiply the values by 256 to give the image a 16-bit range with only 256 bins
var256=256
var8=8
convert $inputImg remap.png -clut -evaluate multiply $var256 -depth $var8 $outImg

exit 0 


