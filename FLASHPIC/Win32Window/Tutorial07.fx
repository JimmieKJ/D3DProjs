//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse1 : register( t0 );
Texture2D txDiffuse1_1 : register(t1);
Texture2D txDiffuse2 : register(t2);
Texture2D txDiffuse2_2 : register(t3);
Texture2D txDiffuse3 : register(t4);
Texture2D txDiffuse3_3 : register(t5);
Texture2D txDiffuse4 : register(t6);
Texture2D txDiffuse4_4 : register(t7);
Texture2D txDiffuse5 : register(t8);
Texture2D txDiffuse5_5: register(t9);
Texture2D txDiffuse6 : register(t10);
Texture2D txDiffuse6_6 : register(t11);

Texture2D txDiffuse13 : register(t12);
Texture2D txDiffuse14 : register(t13);
Texture2D txDiffuse15: register(t14);
Texture2D txDiffuse16 : register(t15);
Texture2D txDiffuse17 : register(t16);
//其实 17个也完全没问题

SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
//	float4 color(0.1,0.1, 0.1,0.2);
	if (vMeshColor.x <0.11)
	{
		return txDiffuse1.Sample(samLinear, input.Tex)*(1, 1 , 1, 1);//显示 A //   *  vMeshColor//如果颜色输出的颜色  透明度是0，那么得到的是黑色的画面渲染画面//死心吧
	}
	else if (vMeshColor.x <0.16)
	{
		return txDiffuse1.Sample(samLinear, input.Tex) ;//	a
	 }
	else if (vMeshColor.x <0.21)
	{
		return txDiffuse2.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.26)
	{
		return txDiffuse2_2.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.31)
	{
		return txDiffuse3.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.36)
	{
		return txDiffuse3_3.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.41)
	{
		return txDiffuse4.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.46)
	{
		return txDiffuse4_4.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.51)
	{
		return txDiffuse5.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.56)
	{
		return txDiffuse5_5.Sample(samLinear, input.Tex) ;//	a
	}
	else if (vMeshColor.x <0.61)
	{
		return txDiffuse6.Sample(samLinear, input.Tex);//	a
	}
	else if (vMeshColor.x <0.66)
	{
		return txDiffuse6_6.Sample(samLinear, input.Tex);//	a
	}
    return txDiffuse6.Sample( samLinear, input.Tex ) ;
}
