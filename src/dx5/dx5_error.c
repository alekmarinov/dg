/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_error.c                                        */
/* Description:   DG/DirectX errors                                  */
/*                                                                   */
/*********************************************************************/

#include "dx5_error.h"

static struct _dx5_error_info 
{
	HRESULT hResult;
	const char* message;
} dx5_error_info[] = 
{
	/* 
	* Undefined error 
	*/
	{-1, "Undefined error"},

	/*
	* Succesful operation
	*/
	{DD_OK, "Operation successful"},

	/*
	* This object is already initialized
	*/
	{DDERR_ALREADYINITIALIZED, "This object is already initialized"},
	/*
	* This surface can not be attached to the requested surface.
	*/
	{DDERR_CANNOTATTACHSURFACE, "This surface can not be attached to the requested surface"},
	/*
	* This surface can not be detached from the requested surface.
	*/
	{DDERR_CANNOTDETACHSURFACE, "This surface can not be detached from the requested surface"},

	/*
	* Support is currently not available.
	*/
	{DDERR_CURRENTLYNOTAVAIL, "Support is currently not available"},

	/*
	* An exception was encountered while performing the requested operation
	*/
	{DDERR_EXCEPTION, "An exception was encountered while performing the requested operation"},

	/*
	* Generic failure.
	*/
	{DDERR_GENERIC, "Generic failure"},

	/*
	* Height of rectangle provided is not a multiple of reqd alignment
	*/
	{DDERR_HEIGHTALIGN, "Height of rectangle provided is not a multiple of reqd alignment"},

	/*
	* Unable to match primary surface creation request with existing
	* primary surface.
	*/
	{DDERR_INCOMPATIBLEPRIMARY, "Unable to match primary surface creation request with existing primary surface"},

	/*
	* One or more of the caps bits passed to the callback are incorrect.
	*/
	{DDERR_INVALIDCAPS, "One or more of the caps bits passed to the callback are incorrect"},

	/*
	* DirectDraw does not support provided Cliplist.
	*/
	{DDERR_INVALIDCLIPLIST, "DirectDraw does not support provided Cliplist"},

	/*
	* DirectDraw does not support the requested mode
	*/
	{DDERR_INVALIDMODE, "DirectDraw does not support the requested mode"},

	/*
	* DirectDraw received a pointer that was an invalid DIRECTDRAW object.
	*/
	{DDERR_INVALIDOBJECT, "DirectDraw received a pointer that was an invalid DIRECTDRAW object"},

	/*
	* One or more of the parameters passed to the callback function are
	* incorrect.
	*/
	{DDERR_INVALIDPARAMS, "One or more of the parameters passed to the callback function are incorrect"},

	/*
	* pixel format was invalid as specified
	*/
	{DDERR_INVALIDPIXELFORMAT, "pixel format was invalid as specified"},

	/*
	* Rectangle provided was invalid.
	*/
	{DDERR_INVALIDRECT, "Rectangle provided was invalid"},

	/*
	* Operation could not be carried out because one or more surfaces are locked
	*/
	{DDERR_LOCKEDSURFACES, "Operation could not be carried out because one or more surfaces are locked"},

	/*
	* There is no 3D present.
	*/
	{DDERR_NO3D, "There is no 3D present"},

	/*
	* Operation could not be carried out because there is no alpha accleration
	* .
	*/
	{DDERR_NOALPHAHW, "Operation could not be carried out because there is no alpha accleration hardware present or available"},

	/*
	* Operation could not be carried out because there is no stereo
	* hardware present or available.
	*/
	{DDERR_NOSTEREOHARDWARE, "Operation could not be carried out because there is no stereo hardware present or available"},

	/*
	* Operation could not be carried out because there is no hardware
	* present which supports stereo surfaces
	*/
	{DDERR_NOSURFACELEFT, "Operation could not be carried out because there is no hardware present which supports stereo surfaces"},

	/*
	* no clip list available
	*/
	{DDERR_NOCLIPLIST, "no clip list available"},

	/*
	* Operation could not be carried out because there is no color conversion
	* hardware present or available.
	*/
	{DDERR_NOCOLORCONVHW, "Operation could not be carried out because there is no color conversion hardware present or available"},

	/*
	* Create function called without DirectDraw object method SetCooperativeLevel
	* being called.
	*/
	{DDERR_NOCOOPERATIVELEVELSET, "Create function called without DirectDraw object method SetCooperativeLevel being called"},

	/*
	* Surface doesn't currently have a color key
	*/
	{DDERR_NOCOLORKEY, "Surface doesn't currently have a color key"},

	/*
	* Operation could not be carried out because there is no hardware support
	* of the dest color key.
	*/
	{DDERR_NOCOLORKEYHW, "Operation could not be carried out because there is no hardware support of the dest color key"},

	/*
	* No DirectDraw support possible with current display driver
	*/
	{DDERR_NODIRECTDRAWSUPPORT, "No DirectDraw support possible with current display driver"},

	/*
	* Operation requires the application to have exclusive mode but the
	* application does not have exclusive mode.
	*/
	{DDERR_NOEXCLUSIVEMODE, "Operation requires the application to have exclusive mode but the application does not have exclusive mode"},

	/*
	* Flipping visible surfaces is not supported.
	*/
	{DDERR_NOFLIPHW, "Flipping visible surfaces is not supported"},

	/*
	* There is no GDI present.
	*/
	{DDERR_NOGDI, "There is no GDI present"},

	/*
	* Operation could not be carried out because there is no hardware present
	* or available.
	*/
	{DDERR_NOMIRRORHW, "Operation could not be carried out because there is no hardware present or available"},

	/*
	* Requested item was not found
	*/
	{DDERR_NOTFOUND, "Requested item was not found"},

	/*
	* Operation could not be carried out because there is no overlay hardware
	* present or available.
	*/
	{DDERR_NOOVERLAYHW, "Operation could not be carried out because there is no overlay hardware present or available"},

	/*
	* Operation could not be carried out because the source and destination
	* rectangles are on the same surface and overlap each other.
	*/
	{DDERR_OVERLAPPINGRECTS, "Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other"},

	/*
	* Operation could not be carried out because there is no appropriate raster
	* op hardware present or available.
	*/
	{DDERR_NORASTEROPHW, "Operation could not be carried out because there is no appropriate raster op hardware present or available"},

	/*
	* Operation could not be carried out because there is no rotation hardware
	* present or available.
	*/
	{DDERR_NOROTATIONHW, "Operation could not be carried out because there is no rotation hardware present or available"},

	/*
	* Operation could not be carried out because there is no hardware support
	* for stretching
	*/
	{DDERR_NOSTRETCHHW, "Operation could not be carried out because there is no hardware support for stretching"},

	/*
	* DirectDrawSurface is not in 4 bit color palette and the requested operation
	* requires 4 bit color palette.
	*/
	{DDERR_NOT4BITCOLOR, "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette"},

	/*
	* DirectDrawSurface is not in 4 bit color index palette and the requested
	* operation requires 4 bit color index palette.
	*/
	{DDERR_NOT4BITCOLORINDEX, "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette"},

	/*
	* DirectDraw Surface is not in 8 bit color mode and the requested operation
	* requires 8 bit color.
	*/
	{DDERR_NOT8BITCOLOR, "DirectDraw Surface is not in 8 bit color mode and the requested operation requires 8 bit color"},

	/*
	* Operation could not be carried out because there is no texture mapping
	* hardware present or available.
	*/
	{DDERR_NOTEXTUREHW, "Operation could not be carried out because there is no texture mapping hardware present or available"},

	/*
	* Operation could not be carried out because there is no hardware support
	* for vertical blank synchronized operations.
	*/
	{DDERR_NOVSYNCHW, "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations"},

	/*
	* Operation could not be carried out because there is no hardware support
	* for zbuffer blting.
	*/
	{DDERR_NOZBUFFERHW, "Operation could not be carried out because there is no hardware support for zbuffer blting"},

	/*
	* Overlay surfaces could not be z layered based on their BltOrder because
	* the hardware does not support z layering of overlays.
	*/
	{DDERR_NOZOVERLAYHW, "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays"},

	/*
	* The hardware needed for the requested operation has already been
	* allocated.
	*/
	{DDERR_OUTOFCAPS, "The hardware needed for the requested operation has already been allocated"},

	/*
	* DirectDraw does not have enough memory to perform the operation.
	*/
	{DDERR_OUTOFMEMORY, "DirectDraw does not have enough memory to perform the operation"},

	/*
	* DirectDraw does not have enough memory to perform the operation.
	*/
	{DDERR_OUTOFVIDEOMEMORY, "DirectDraw does not have enough memory to perform the operation"},

	/*
	* hardware does not support clipped overlays
	*/
	{DDERR_OVERLAYCANTCLIP, "hardware does not support clipped overlays"},

	/*
	* Can only have ony color key active at one time for overlays
	*/
	{DDERR_OVERLAYCOLORKEYONLYONEACTIVE, "Can only have ony color key active at one time for overlays"},

	/*
	* Access to this palette is being refused because the palette is already
	* locked by another thread.
	*/
	{DDERR_PALETTEBUSY, "Access to this palette is being refused because the palette is already locked by another thread"},

	/*
	* No src color key specified for this operation.
	*/
	{DDERR_COLORKEYNOTSET, "No src color key specified for this operation"},

	/*
	* This surface is already attached to the surface it is being attached to.
	*/
	{DDERR_SURFACEALREADYATTACHED, "This surface is already attached to the surface it is being attached to"},

	/*
	* This surface is already a dependency of the surface it is being made a
	* dependency of.
	*/
	{DDERR_SURFACEALREADYDEPENDENT, "This surface is already a dependency of the surface it is being made a dependency of"},

	/*
	* Access to this surface is being refused because the surface is already
	* locked by another thread.
	*/
	{DDERR_SURFACEBUSY, "Access to this surface is being refused because the surface is already locked by another thread"},

	/*
	* Access to this surface is being refused because no driver exists
	* which can supply a pointer to the surface.
	* This is most likely to happen when attempting to lock the primary
	* surface when no DCI provider is present.
	* Will also happen on attempts to lock an optimized surface.
	*/
	{DDERR_CANTLOCKSURFACE, "Access to this surface is being refused because no driver exists which can supply a pointer to the surface. This is most likely to happen when attempting to lock the primary surface when no DCI provider is present. Will also happen on attempts to lock an optimized surface."},

	/*
	* Access to Surface refused because Surface is obscured.
	*/
	{DDERR_SURFACEISOBSCURED, "Access to Surface refused because Surface is obscured"},

	/*
	* Access to this surface is being refused because the surface is gone.
	* The DIRECTDRAWSURFACE object representing this surface should
	* have Restore called on it.
	*/
	{DDERR_SURFACELOST, "Access to this surface is being refused because the surface is gone. The DIRECTDRAWSURFACE object representing this surface should have Restore called on it."},

	/*
	* The requested surface is not attached.
	*/
	{DDERR_SURFACENOTATTACHED, "The requested surface is not attached"},

	/*
	* Height requested by DirectDraw is too large.
	*/
	{DDERR_TOOBIGHEIGHT, "Height requested by DirectDraw is too large"},

	/*
	* Size requested by DirectDraw is too large --  The individual height and
	* width are OK.
	*/
	{DDERR_TOOBIGSIZE, "Size requested by DirectDraw is too large --  The individual height and width are OK"},

	/*
	* Width requested by DirectDraw is too large.
	*/
	{DDERR_TOOBIGWIDTH, "Width requested by DirectDraw is too large"},

	/*
	* Action not supported.
	*/
	{DDERR_UNSUPPORTED, "Action not supported"},

	/*
	* Pixel format requested is unsupported by DirectDraw
	*/
	{DDERR_UNSUPPORTEDFORMAT, "Pixel format requested is unsupported by DirectDraw"},

	/*
	* Bitmask in the pixel format requested is unsupported by DirectDraw
	*/
	{DDERR_UNSUPPORTEDMASK, "Bitmask in the pixel format requested is unsupported by DirectDraw"},

	/*
	* The specified stream contains invalid data
	*/
	{DDERR_INVALIDSTREAM, "The specified stream contains invalid data"},

	/*
	* vertical blank is in progress
	*/
	{DDERR_VERTICALBLANKINPROGRESS, "vertical blank is in progress"},

	/*
	* Informs DirectDraw that the previous Blt which is transfering information
	* to or from this Surface is incomplete.
	*/
	{DDERR_WASSTILLDRAWING, "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete"},

	/*
	* The specified surface type requires specification of the COMPLEX flag
	*/
	{DDERR_DDSCAPSCOMPLEXREQUIRED, "The specified surface type requires specification of the COMPLEX flag"},

	/*
	* Rectangle provided was not horizontally aligned on reqd. boundary
	*/
	{DDERR_XALIGN, "Rectangle provided was not horizontally aligned on reqd. boundary"},

	/*
	* The GUID passed to DirectDrawCreate is not a valid DirectDraw driver
	* identifier.
	*/
	{DDERR_INVALIDDIRECTDRAWGUID, "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier"},

	/*
	* A DirectDraw object representing this driver has already been created
	* for this process.
	*/
	{DDERR_DIRECTDRAWALREADYCREATED, "A DirectDraw object representing this driver has already been created for this process"},

	/*
	* A hardware only DirectDraw object creation was attempted but the driver
	* did not support any hardware.
	*/
	{DDERR_NODIRECTDRAWHW, "A hardware only DirectDraw object creation was attempted but the driver did not support any hardware"},

	/*
	* this process already has created a primary surface
	*/
	{DDERR_PRIMARYSURFACEALREADYEXISTS, "this process already has created a primary surface"},

	/*
	* software emulation not available.
	*/
	{DDERR_NOEMULATION, "software emulation not available"},

	/*
	* region passed to Clipper::GetClipList is too small.
	*/
	{DDERR_REGIONTOOSMALL, "region passed to Clipper::GetClipList is too small"},

	/*
	* an attempt was made to set a clip list for a clipper objec that
	* is already monitoring an hwnd.
	*/
	{DDERR_CLIPPERISUSINGHWND, "an attempt was made to set a clip list for a clipper objec that is already monitoring an hwnd"},

	/*
	* No clipper object attached to surface object
	*/
	{DDERR_NOCLIPPERATTACHED, "No clipper object attached to surface object"},

	/*
	* Clipper notification requires an HWND or
	* no HWND has previously been set as the CooperativeLevel HWND.
	*/
	{DDERR_NOHWND, "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND"},

	/*
	* HWND used by DirectDraw CooperativeLevel has been subclassed,
	* this prevents DirectDraw from restoring state.
	*/
	{DDERR_HWNDSUBCLASSED, "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state"},

	/*
	* The CooperativeLevel HWND has already been set.
	* It can not be reset while the process has surfaces or palettes created.
	*/
	{DDERR_HWNDALREADYSET, "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created."},

	/*
	* No palette object attached to this surface.
	*/
	{DDERR_NOPALETTEATTACHED, "No palette object attached to this surface"},

	/*
	* No hardware support for 16 or 256 color palettes.
	*/
	{DDERR_NOPALETTEHW, "No hardware support for 16 or 256 color palettes"},

	/*
	* If a clipper object is attached to the source surface passed into a
	* BltFast call.
	*/
	{DDERR_BLTFASTCANTCLIP, "If a clipper object is attached to the source surface passed into a BltFast call"},

	/*
	* No blter.
	*/
	{DDERR_NOBLTHW, "No blter"},

	/*
	* No DirectDraw ROP hardware.
	*/
	{DDERR_NODDROPSHW, "No DirectDraw ROP hardware"},

	/*
	* returned when GetOverlayPosition is called on a hidden overlay
	*/
	{DDERR_OVERLAYNOTVISIBLE, "returned when GetOverlayPosition is called on a hidden overlay"},

	/*
	* returned when GetOverlayPosition is called on a overlay that UpdateOverlay
	* has never been called on to establish a destionation.
	*/
	{DDERR_NOOVERLAYDEST, "returned when GetOverlayPosition is called on a overlay that UpdateOverlay has never been called on to establish a destionation"},

	/*
	* returned when the position of the overlay on the destionation is no longer
	* legal for that destionation.
	*/
	{DDERR_INVALIDPOSITION, "returned when the position of the overlay on the destionation is no longer legal for that destionation"},

	/*
	* returned when an overlay member is called for a non-overlay surface
	*/
	{DDERR_NOTAOVERLAYSURFACE, "returned when an overlay member is called for a non-overlay surface"},

	/*
	* An attempt was made to set the cooperative level when it was already
	* set to exclusive.
	*/
	{DDERR_EXCLUSIVEMODEALREADYSET, "An attempt was made to set the cooperative level when it was already set to exclusive"},

	/*
	* An attempt has been made to flip a surface that is not flippable.
	*/
	{DDERR_NOTFLIPPABLE, "An attempt has been made to flip a surface that is not flippable"},

	/*
	* Can't duplicate primary & 3D surfaces, or surfaces that are implicitly
	* created.
	*/
	{DDERR_CANTDUPLICATE, "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created"},

	/*
	* Surface was not locked.  An attempt to unlock a surface that was not
	* locked at all, or by this process, has been attempted.
	*/
	{DDERR_NOTLOCKED, "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted."},

	/*
	* Windows can not create any more DCs, or a DC was requested for a paltte-indexed
	* surface when the surface had no palette AND the display mode was not palette-indexed
	* (in this case DirectDraw cannot select a proper palette into the DC)
	*/
	{DDERR_CANTCREATEDC, "Windows can not create any more DCs, or a DC was requested for a paltte-indexed surface when the surface had no palette AND the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC)"},

	/*
	* No DC was ever created for this surface.
	*/
	{DDERR_NODC, "No DC was ever created for this surface"},

	/*
	* This surface can not be restored because it was created in a different
	* mode.
	*/
	{DDERR_WRONGMODE, "This surface can not be restored because it was created in a different mode"},

	/*
	* This surface can not be restored because it is an implicitly created
	* surface.
	*/
	{DDERR_IMPLICITLYCREATED, "This surface can not be restored because it is an implicitly created surface"},

	/*
	* The surface being used is not a palette-based surface
	*/
	{DDERR_NOTPALETTIZED, "The surface being used is not a palette-based surface"},


	/*
	* The display is currently in an unsupported mode
	*/
	{DDERR_UNSUPPORTEDMODE, "The display is currently in an unsupported mode"},

	/*
	* Operation could not be carried out because there is no mip-map
	* texture mapping hardware present or available.
	*/
	{DDERR_NOMIPMAPHW, "Operation could not be carried out because there is no mip-map texture mapping hardware present or available"},

	/*
	* The requested action could not be performed because the surface was of
	* the wrong type.
	*/
	{DDERR_INVALIDSURFACETYPE, "The requested action could not be performed because the surface was of the wrong type"},


	/*
	* Device does not support optimized surfaces, therefore no video memory optimized surfaces
	*/
	{DDERR_NOOPTIMIZEHW, "Device does not support optimized surfaces, therefore no video memory optimized surfaces"},

	/*
	* Surface is an optimized surface, but has not yet been allocated any memory
	*/
	{DDERR_NOTLOADED, "Surface is an optimized surface, but has not yet been allocated any memory"},

	/*
	* Attempt was made to create or set a device window without first setting
	* the focus window
	*/
	{DDERR_NOFOCUSWINDOW, "Attempt was made to create or set a device window without first setting the focus window"},

	/*
	* Attempt was made to set a palette on a mipmap sublevel
	*/
	{DDERR_NOTONMIPMAPSUBLEVEL, "Attempt was made to set a palette on a mipmap sublevel"},

	/*
	* A DC has already been returned for this surface. Only one DC can be
	* retrieved per surface.
	*/
	{DDERR_DCALREADYCREATED, "A DC has already been returned for this surface. Only one DC can be retrieved per surface"},

	/*
	* An attempt was made to allocate non-local video memory from a device
	* that does not support non-local video memory.
	*/
	{DDERR_NONONLOCALVIDMEM, "An attempt was made to allocate non-local video memory from a device that does not support non-local video memory"},

	/*
	* The attempt to page lock a surface failed.
	*/
	{DDERR_CANTPAGELOCK, "The attempt to page lock a surface failed"},

	/*
	* The attempt to page unlock a surface failed.
	*/
	{DDERR_CANTPAGEUNLOCK, "The attempt to page unlock a surface failed"},

	/*
	* An attempt was made to page unlock a surface with no outstanding page locks.
	*/
	{DDERR_NOTPAGELOCKED, "An attempt was made to page unlock a surface with no outstanding page locks"},

	/*
	* There is more data available than the specified buffer size could hold
	*/
	{DDERR_MOREDATA, "There is more data available than the specified buffer size could hold"},

	/*
	* The data has expired and is therefore no longer valid.
	*/
	{DDERR_EXPIRED, "The data has expired and is therefore no longer valid"},

	/*
	* The mode test has finished executing.
	*/
	{DDERR_TESTFINISHED, "The mode test has finished executing"},

	/*
	* The mode test has switched to a new mode.
	*/
	{DDERR_NEWMODE, "The mode test has switched to a new mode"},

	/*
	* D3D has not yet been initialized.
	*/
	{DDERR_D3DNOTINITIALIZED, "D3D has not yet been initialized"},

	/*
	* The video port is not active
	*/
	{DDERR_VIDEONOTACTIVE, "The video port is not active"},

	/*
	* The monitor does not have EDID data.
	*/
	{DDERR_NOMONITORINFORMATION, "The monitor does not have EDID data"},

	/*
	* The driver does not enumerate display mode refresh rates.
	*/
	{DDERR_NODRIVERSUPPORT, "The driver does not enumerate display mode refresh rates"},

	/*
	* Surfaces created by one direct draw device cannot be used directly by
	* another direct draw device.
	*/
	{DDERR_DEVICEDOESNTOWNSURFACE, "Surfaces created by one direct draw device cannot be used directly by another direct draw device"},

	/*
	* An attempt was made to invoke an interface member of a DirectDraw object
	* created by CoCreateInstance() before it was initialized.
	*/
	{DDERR_NOTINITIALIZED, "An attempt was made to invoke an interface member of a DirectDraw object created by CoCreateInstance() before it was initialized"}
};

const char* dx5_error_message(HRESULT hResult)
{
	struct _dx5_error_info* dxError = dx5_error_info;
	while (dxError->hResult && dxError->hResult != hResult) dxError++;
	if (dxError->hResult == hResult)
	{
		return dxError->message;
	}
	return dx5_error_info[0].message;
}
