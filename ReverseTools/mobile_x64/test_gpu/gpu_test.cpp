// [SDL 08/09 pha 3] Thu SDL_GPU tren may nay: tao thiet bi (SPIR-V -> Vulkan), claim cua so, xoa man 60 khung, kiem dinh dang texture.
#include <SDL3/SDL.h>
#include <stdio.h>
int main( int argc, char **argv )
{
	if ( !SDL_Init( SDL_INIT_VIDEO ) ) { printf( "SDL_Init loi: %s\n", SDL_GetError() ); return 1; }
	SDL_Window *w = SDL_CreateWindow( "gpu_test", 640, 480, 0 );
	if ( !w ) { printf( "CreateWindow loi: %s\n", SDL_GetError() ); return 1; }
	SDL_GPUDevice *dev = SDL_CreateGPUDevice( SDL_GPU_SHADERFORMAT_SPIRV, false, NULL );
	if ( !dev ) { printf( "CreateGPUDevice(SPIRV) loi: %s\n", SDL_GetError() ); return 1; }
	printf( "driver: %s | shader formats: 0x%x\n", SDL_GetGPUDeviceDriver( dev ), ( unsigned )SDL_GetGPUShaderFormats( dev ) );
	if ( !SDL_ClaimWindowForGPUDevice( dev, w ) ) { printf( "ClaimWindow loi: %s\n", SDL_GetError() ); return 1; }
	printf( "swapchain format: %d | vsync ok=%d immediate ok=%d mailbox ok=%d\n", ( int )SDL_GetGPUSwapchainTextureFormat( dev, w ),
		( int )SDL_WindowSupportsGPUPresentMode( dev, w, SDL_GPU_PRESENTMODE_VSYNC ), ( int )SDL_WindowSupportsGPUPresentMode( dev, w, SDL_GPU_PRESENTMODE_IMMEDIATE ),
		( int )SDL_WindowSupportsGPUPresentMode( dev, w, SDL_GPU_PRESENTMODE_MAILBOX ) );
	struct { const char *n; SDL_GPUTextureFormat f; } fm[] = {
		{ "B8G8R8A8", SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM }, { "R8G8B8A8", SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM }, { "B5G6R5", SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM },
		{ "B5G5R5A1", SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM }, { "B4G4R4A4", SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM }, { "R8G8", SDL_GPU_TEXTUREFORMAT_R8G8_UNORM },
		{ "R8", SDL_GPU_TEXTUREFORMAT_R8_UNORM }, { "A8", SDL_GPU_TEXTUREFORMAT_A8_UNORM } };
	for ( int i = 0; i < 8; i++ )
		printf( "  %-9s sampler=%d colortarget=%d\n", fm[i].n, ( int )SDL_GPUTextureSupportsFormat( dev, fm[i].f, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_SAMPLER ),
			( int )SDL_GPUTextureSupportsFormat( dev, fm[i].f, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_COLOR_TARGET ) );
	Uint64 t0 = SDL_GetTicks(); int n = 0; Uint32 sww = 0, swh = 0;
	for ( ; n < 60; n++ )
	{
		SDL_Event e; while ( SDL_PollEvent( &e ) ) {}
		SDL_GPUCommandBuffer *cb = SDL_AcquireGPUCommandBuffer( dev );
		if ( !cb ) { printf( "AcquireGPUCommandBuffer loi: %s\n", SDL_GetError() ); break; }
		SDL_GPUTexture *sw = NULL;
		if ( !SDL_WaitAndAcquireGPUSwapchainTexture( cb, w, &sw, &sww, &swh ) ) { printf( "AcquireSwapchainTexture loi: %s\n", SDL_GetError() ); SDL_CancelGPUCommandBuffer( cb ); break; }
		if ( sw )
		{
			SDL_GPUColorTargetInfo ci; SDL_zero( ci ); ci.texture = sw; ci.clear_color.r = 0.1f; ci.clear_color.g = 0.4f; ci.clear_color.b = ( n % 2 ) ? 0.8f : 0.2f; ci.clear_color.a = 1.0f;
			ci.load_op = SDL_GPU_LOADOP_CLEAR; ci.store_op = SDL_GPU_STOREOP_STORE;
			SDL_GPURenderPass *rp = SDL_BeginGPURenderPass( cb, &ci, 1, NULL );
			SDL_EndGPURenderPass( rp );
		}
		SDL_SubmitGPUCommandBuffer( cb );
	}
	printf( "60 khung xoa man: %llu ms (%s)\n", ( unsigned long long )( SDL_GetTicks() - t0 ), sww ? "co swapchain" : "khong co swapchain" );
	SDL_ReleaseWindowFromGPUDevice( dev, w ); SDL_DestroyGPUDevice( dev ); SDL_DestroyWindow( w ); SDL_Quit();
	printf( "PASS gpu_test\n" );
	return 0;
}
