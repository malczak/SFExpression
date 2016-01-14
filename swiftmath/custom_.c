//
//  custom_.c
//  swiftmath
//
//  Created by malczak on 12/01/16.
//  Copyright © 2016 ThePirateCat. All rights reserved.
//

#include "custom_.h"
#include <math.h>

sfarg *sf_magick2( sfarg * const p, void *payload ) /*  / */
{
    sfvalue(p) = sin( sfvalue( sfaram2(p) ) ) + sfvalue( sfaram1(p) );
    return sfaram2(p);
};

