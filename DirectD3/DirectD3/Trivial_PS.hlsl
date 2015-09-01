struct INPUT_PIXEL
{
	float3 colorOut : COLOR;
	float4 projectedCoordinate : SV_POSITION;
};

float4 main(INPUT_PIXEL input) : SV_TARGET
{
	return float4(input.colorOut, 1);
}