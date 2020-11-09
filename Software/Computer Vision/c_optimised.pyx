import numpy as np

cpdef list find_groups(unsigned char[:, :] mask):
    '''
    Finds groups of connected pixels in a 2d image, returns a list of groups of pixels
    '''
    cdef int ylen, xlen, y, x

    ylen, xlen = mask.shape[0], mask.shape[1]

    groups = np.zeros((ylen, xlen), dtype=np.uint8)
    groupIndex = [[], ]
    
    for y in range(ylen):
        for x in range(xlen):
            if mask[y, x] == 255:
                # this pixel is white

                if x > 0 and y > 0:
                    # not at the borders

                    if groups[y-1, x] != 0:
                        # pixel above has a group
                        # add this pixel to that group
                        groups[y, x] = groups[y-1, x]
                        groupIndex[groups[y, x]].append([y, x])

                        if groups[y, x-1] != groups[y-1, x] and groups[y, x-1] != 0:
                            # pixel to the left has a different group
                            newgroup = groups[y-1, x]
                            oldgroup = groups[y, x-1]
                            # merge the two groups
                            for pixel in groupIndex[oldgroup]:
                                groups[pixel[0], pixel[1]] = newgroup

                            groupIndex[newgroup] += groupIndex[oldgroup]
                            groupIndex[oldgroup] = []

                    elif groups[y, x-1] != 0:
                        # pixel to left has a group and above doesn't
                        # add this pixel to the group
                        groups[y, x] = groups[y, x-1]

                    elif groups[y, x] == 0:
                        groups[y, x] = len(groupIndex)
                        groupIndex.append([[y, x]])
    
    return groupIndex