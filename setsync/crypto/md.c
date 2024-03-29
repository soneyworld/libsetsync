/**
 * \file md.c
 * 
 * \brief Generic message digest wrapper for PolarSSL
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "md.h"
#include "md_wrap.h"

#include <stdlib.h>

#if defined _MSC_VER && !defined strcasecmp
#define strcasecmp _stricmp
#endif

static const int supported_digests[] = {

        POLARSSL_MD_MD2,

        POLARSSL_MD_MD4,

        POLARSSL_MD_MD5,

        POLARSSL_MD_SHA1,

        POLARSSL_MD_SHA224,
        POLARSSL_MD_SHA256,

        POLARSSL_MD_SHA384,
        POLARSSL_MD_SHA512,

        0
};

const int *md_list( void )
{
    return supported_digests;
}

const md_info_t *md_info_from_string( const char *md_name )
{
    if( NULL == md_name )
        return NULL;

    /* Get the appropriate digest information */
    if( !strcasecmp( "MD2", md_name ) )
        return md_info_from_type( POLARSSL_MD_MD2 );
    if( !strcasecmp( "MD4", md_name ) )
        return md_info_from_type( POLARSSL_MD_MD4 );
    if( !strcasecmp( "MD5", md_name ) )
        return md_info_from_type( POLARSSL_MD_MD5 );
    if( !strcasecmp( "SHA1", md_name ) || !strcasecmp( "SHA", md_name ) )
        return md_info_from_type( POLARSSL_MD_SHA1 );
    if( !strcasecmp( "SHA224", md_name ) )
        return md_info_from_type( POLARSSL_MD_SHA224 );
    if( !strcasecmp( "SHA256", md_name ) )
        return md_info_from_type( POLARSSL_MD_SHA256 );
    if( !strcasecmp( "SHA384", md_name ) )
        return md_info_from_type( POLARSSL_MD_SHA384 );
    if( !strcasecmp( "SHA512", md_name ) )
        return md_info_from_type( POLARSSL_MD_SHA512 );
    return NULL;
}

const md_info_t *md_info_from_type( md_type_t md_type )
{
    switch( md_type )
    {
        case POLARSSL_MD_MD2:
            return &md2_info;
        case POLARSSL_MD_MD4:
            return &md4_info;
        case POLARSSL_MD_MD5:
            return &md5_info;
        case POLARSSL_MD_SHA1:
            return &sha1_info;
        case POLARSSL_MD_SHA224:
            return &sha224_info;
        case POLARSSL_MD_SHA256:
            return &sha256_info;
        case POLARSSL_MD_SHA384:
            return &sha384_info;
        case POLARSSL_MD_SHA512:
            return &sha512_info;
        default:
            return NULL;
    }
}

int md_init_ctx( md_context_t *ctx, const md_info_t *md_info )
{
    if( md_info == NULL || ctx == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    memset( ctx, 0, sizeof( md_context_t ) );

    if( ( ctx->md_ctx = md_info->ctx_alloc_func() ) == NULL )
        return POLARSSL_ERR_MD_ALLOC_FAILED;

    ctx->md_info = md_info;

    md_info->starts_func( ctx->md_ctx );

    return 0;
}

int md_free_ctx( md_context_t *ctx )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->ctx_free_func( ctx->md_ctx );
    ctx->md_ctx = NULL;

    return 0;
}

int md_starts( md_context_t *ctx )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->starts_func( ctx->md_ctx );

    return 0;
}

int md_update( md_context_t *ctx, const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->update_func( ctx->md_ctx, input, ilen );

    return 0;
}

int md_finish( md_context_t *ctx, unsigned char *output )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->finish_func( ctx->md_ctx, output );

    return 0;
}

int md( const md_info_t *md_info, const unsigned char *input, size_t ilen,
            unsigned char *output )
{
    if ( md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    md_info->digest_func( input, ilen, output );

    return 0;
}

int md_file( const md_info_t *md_info, const char *path, unsigned char *output )
{
#if defined(POLARSSL_FS_IO)
    int ret;
#endif

    if( md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

#if defined(POLARSSL_FS_IO)
    ret = md_info->file_func( path, output );
    if( ret != 0 )
        return( POLARSSL_ERR_MD_FILE_IO_ERROR + ret );

    return( ret );
#else
    ((void) path);
    ((void) output);

    return POLARSSL_ERR_MD_FEATURE_UNAVAILABLE;
#endif
}

int md_hmac_starts( md_context_t *ctx, const unsigned char *key, size_t keylen )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->hmac_starts_func( ctx->md_ctx, key, keylen);

    return 0;
}

int md_hmac_update( md_context_t *ctx, const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->hmac_update_func( ctx->md_ctx, input, ilen );

    return 0;
}

int md_hmac_finish( md_context_t *ctx, unsigned char *output)
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->hmac_finish_func( ctx->md_ctx, output);

    return 0;
}

int md_hmac_reset( md_context_t *ctx )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    ctx->md_info->hmac_reset_func( ctx->md_ctx);

    return 0;
}

int md_hmac( const md_info_t *md_info, const unsigned char *key, size_t keylen,
                const unsigned char *input, size_t ilen,
                unsigned char *output )
{
    if( md_info == NULL )
        return POLARSSL_ERR_MD_BAD_INPUT_DATA;

    md_info->hmac_func( key, keylen, input, ilen, output );

    return 0;
}

