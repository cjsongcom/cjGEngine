//
//  Shader.fsh
//  HelloCJApp
//
//  Created by 송천종 on 14. 2. 1..
//  Copyright (c) 2014년 송천종. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
