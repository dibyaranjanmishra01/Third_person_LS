struct Input
{
};

struct Output
{
	float4 Color : SV_Target0;
};

cbuffer ColorData : register(b0, space3)
{
	float4 color;
};

Output main(Input input)
{
	Output output;
	output.Color = color;

	return output;
}
