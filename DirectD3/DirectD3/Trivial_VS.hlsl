#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
};

struct OUTPUT_VERTEX
{
	float4 posH : SV_POSITION;
	float4 Color : COLOR;
	float2 UV : UVCOORD;
	float3 norm : NORMAL;
};

// TODO: PART 3 STEP 2a
cbuffer THIS_IS_VRAM : register( b0 )
{
	float4 constantColor;
	float2 constantOffset;
	float2 padding;
};
cbuffer Object : register(b1)
{
	float4x4 SV_WorldMatrix;
	float4x4 SV_ProjectionMatrix;
	float4x4 SV_ViewMatrix;
};
//cbuffer Scene : register(b2)
//{
//	float4x4 SV_ProjectionMatrix;
//	float4x4 SV_ViewMatrix;
//};

OUTPUT_VERTEX main( INPUT_VERTEX fromVertexBuffer )
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 Vector = float4(fromVertexBuffer.coordinate, 1);
	Vector = mul(Vector, SV_WorldMatrix);
	Vector = mul(Vector, SV_ViewMatrix);
	Vector = mul(Vector, SV_ProjectionMatrix);

	sendToRasterizer.posH = Vector;
	sendToRasterizer.UV = fromVertexBuffer.UV;
	//sendToRasterizer.UV = float3(0.25,0.25,0);
	//sendToRasterizer.UV.y = 1 - fromVertexBuffer.UV.y;

	sendToRasterizer.norm = fromVertexBuffer.norm;
	//sendToRasterizer.norm.y = 1 - fromVertexBuffer.norm.y;
	
	return sendToRasterizer;
}