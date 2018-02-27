#include "Common_3/OS/Interfaces/ILogManager.h"
#include "Common_3/OS/Interfaces/IMemoryManager.h"

#include "Scene.h"

uint32 MaterialPropertyGetIndex( uint32_t packed ) {
    const uint32_t valueIndex = ( packed >> 8 ) & 0x0fff;
    return valueIndex;
}

apemodefb::EValueTypeFb MaterialPropertyGetType( uint32_t packed ) {
    const uint32_t valueType = packed & 0x000f;
    return apemodefb::EValueTypeFb( valueType );
}

template < typename T >
typename flatbuffers::Vector< T >::return_type FlatbuffersTVectorGetAtIndex( const flatbuffers::Vector< T > *pVector,
                                                                             const size_t                    itemIndex ) {
    assert( pVector );
    const size_t vectorSize = pVector->size( );

    assert( vectorSize > itemIndex );
    return pVector->operator[]( static_cast< flatbuffers::uoffset_t >( itemIndex ) );
}

std::string GetStringProperty( const apemodefb::SceneFb *pScene, uint32_t valueId ) {
    assert( pScene );
    assert( apemodefb::EValueTypeFb_String == MaterialPropertyGetType( valueId ) );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return FlatbuffersTVectorGetAtIndex( pScene->string_values( ), valueIndex )->str( );
}

bool GetBoolProperty( const apemodefb::SceneFb *pScene, uint32_t valueId ) {
    assert( pScene );
    assert( apemodefb::EValueTypeFb_Bool == MaterialPropertyGetType( valueId ) );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return FlatbuffersTVectorGetAtIndex( pScene->bool_values( ), valueIndex );
}

float GetScalarProperty( const apemodefb::SceneFb *pScene, uint32_t valueId ) {
    assert( pScene );
    assert( apemodefb::EValueTypeFb_Float == MaterialPropertyGetType( valueId ) );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex );
}

vec2 GetVec2Property( const apemodefb::SceneFb *pScene, uint32_t valueId ) {
    assert( pScene );

    const auto valueType = MaterialPropertyGetType( valueId );
    assert( apemodefb::EValueTypeFb_Float2 == valueType );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return vec2( FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex ),
                 FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 1 ) );
}

vec3 GetVec3Property( const apemodefb::SceneFb *pScene, uint32_t valueId ) {
    assert( pScene );

    const auto valueType = MaterialPropertyGetType( valueId );
    assert( apemodefb::EValueTypeFb_Float3 == valueType );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return vec3( FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex ),
                 FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 1 ),
                 FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 2 ) );
}

vec4 GetVec4Property( const apemodefb::SceneFb *pScene, uint32_t valueId, float defaultW = 1.0f ) {
    assert( pScene );

    const auto valueType = MaterialPropertyGetType( valueId );
    assert( apemodefb::EValueTypeFb_Float3 == valueType || apemodefb::EValueTypeFb_Float4 == valueType );

    const uint32_t valueIndex = MaterialPropertyGetIndex( valueId );
    return vec4( FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex ),
                 FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 1 ),
                 FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 2 ),
                 apemodefb::EValueTypeFb_Float4 == valueType
                     ? FlatbuffersTVectorGetAtIndex( pScene->float_values( ), valueIndex + 3 )
                     : defaultW);
}

bool SceneNodeTransform::Validate( ) const {
    return false == ( isnan( translation.getX( ) ) || isnan( translation.getY( ) ) || isnan( translation.getZ( ) ) ||
                      isnan( rotationOffset.getX( ) ) || isnan( rotationOffset.getY( ) ) || isnan( rotationOffset.getZ( ) ) ||
                      isnan( rotationPivot.getX( ) ) || isnan( rotationPivot.getY( ) ) || isnan( rotationPivot.getZ( ) ) ||
                      isnan( preRotation.getX( ) ) || isnan( preRotation.getY( ) ) || isnan( preRotation.getZ( ) ) ||
                      isnan( postRotation.getX( ) ) || isnan( postRotation.getY( ) ) || isnan( postRotation.getZ( ) ) ||
                      isnan( rotation.getX( ) ) || isnan( rotation.getY( ) ) || isnan( rotation.getZ( ) ) ||
                      isnan( scalingOffset.getX( ) ) || isnan( scalingOffset.getY( ) ) || isnan( scalingOffset.getZ( ) ) ||
                      isnan( scalingPivot.getX( ) ) || isnan( scalingPivot.getY( ) ) || isnan( scalingPivot.getZ( ) ) ||
                      isnan( scaling.getX( ) ) || isnan( scaling.getY( ) ) || isnan( scaling.getZ( ) ) ||
                      isnan( geometricTranslation.getX( ) ) || isnan( geometricTranslation.getY( ) ) ||
                      isnan( geometricTranslation.getZ( ) ) || isnan( geometricRotation.getX( ) ) ||
                      isnan( geometricRotation.getY( ) ) || isnan( geometricRotation.getZ( ) ) ||
                      isnan( geometricScaling.getX( ) ) || isnan( geometricScaling.getY( ) ) ||
                      isnan( geometricScaling.getZ( ) ) || scaling.getX( ) == 0 || scaling.getY( ) == 0 ||
                      scaling.getZ( ) == 0 || geometricScaling.getX( ) == 0 || geometricScaling.getY( ) == 0 ||
                      geometricScaling.getZ( ) == 0 );
}

/**
 * Calculates local matrix.
 * @note Contributes node world matrix.
 * @return Node local matrix.
 **/

mat4 SceneNodeTransform::CalculateLocalMatrix( ) const {
    return mat4::translation( translation ) * mat4::translation( rotationOffset ) * mat4::translation( rotationPivot ) *
           mat4::rotationZYX( preRotation ) * mat4::rotationZYX( rotation ) * mat4::rotationZYX( postRotation ) *
           mat4::translation( -rotationPivot ) * mat4::translation( scalingOffset ) * mat4::translation( scalingPivot ) *
           mat4::scale( scaling ) * mat4::translation( -scalingPivot );
}

/**
 * Calculate geometric transform.
 * @note That is an object-offset 3ds Max concept,
 *       it does not influence scene hierarchy, though
 *       it contributes to node world transform.
 * @return Node geometric transform.
 **/

mat4 SceneNodeTransform::CalculateGeometricMatrix( ) const {
    return mat4::translation( geometricTranslation ) * mat4::rotationZYX( geometricRotation ) * mat4::scale( geometricScaling );
}

/**
 * Internal usage only.
 * @see UpdateMatrices().
 * @todo Safety assertions.
 **/

void Scene::UpdateChildWorldMatrices( const uint32_t nodeId ) {
    for ( const auto childId : nodes[ nodeId ].childIds ) {
        localMatrices[ childId ]        = transforms[ childId ].CalculateLocalMatrix( );
        geometricMatrices[ childId ]    = transforms[ childId ].CalculateGeometricMatrix( );
        hierarchicalMatrices[ childId ] = hierarchicalMatrices[ nodes[ childId ].parentId ] * localMatrices[ childId ];
        worldMatrices[ childId ]        = hierarchicalMatrices[ childId ] * geometricMatrices[ childId ];

        if ( false == nodes[ childId ].childIds.empty( ) )
            UpdateChildWorldMatrices( childId );
    }
}

/**
 * Update matrices storage with up-to-date values.
 * @todo Non-recursive, no dynamic memory?
 *       Currently, I tried to avoid any stack memory pollution,
 *       tried to avoid any local variable but kept it clean.
 **/

void Scene::UpdateMatrices( ) {
    if ( transforms.empty( ) || nodes.empty( ) )
        return;

    //
    // Resize matrices storage if needed
    //

    if ( localMatrices.size( ) < transforms.size( ) ) {
        localMatrices.resize( transforms.size( ) );
        worldMatrices.resize( transforms.size( ) );
        geometricMatrices.resize( transforms.size( ) );
        hierarchicalMatrices.resize( transforms.size( ) );
    }

    //
    // Implicit world calculations for the root node.
    //

    localMatrices[ 0 ]        = transforms[ 0 ].CalculateLocalMatrix( );
    geometricMatrices[ 0 ]    = transforms[ 0 ].CalculateGeometricMatrix( );
    hierarchicalMatrices[ 0 ] = localMatrices[ 0 ];
    worldMatrices[ 0 ]        = localMatrices[ 0 ] * geometricMatrices[ 0 ];

    //
    // Start recursive updates from root node.
    //

    UpdateChildWorldMatrices( 0 );
}

std::unique_ptr< Scene > LoadSceneFromFile( const char *filename ) {
    File sceneFile;
    sceneFile.Open( filename, FM_ReadBinary, FSR_Meshes );

    String sceneFileString = sceneFile.ReadText( );

    std::unique_ptr< Scene > scene( conf_placement_new< Scene >( conf_calloc(1,  sizeof( Scene ) ) ) );

    if ( scene->sourceScene = apemodefb::GetSceneFb( sceneFileString.c_str( ) ) ) {

        std::set< uint32 > meshIds;
        std::set< uint32 > materialIds;
        std::map< uint32, Texture * > textureIdToTexture;

        if ( auto nodesFb = scene->sourceScene->nodes( ) ) {

            scene->nodes.resize( nodesFb->size( ) );
            scene->transforms.resize( nodesFb->size( ) );

            const float toRadsFactor = float( PI ) / 180.0f;


            for ( auto nodeFb : *nodesFb ) {
                assert( nodeFb );

                auto &node = scene->nodes[ nodeFb->id( ) ];

                node.id     = nodeFb->id( );
                node.scene  = scene.get( );
                node.meshId = nodeFb->mesh_id( );

                LOGINFOFF( "Processing node: {}", GetStringProperty( scene->sourceScene, nodeFb->name_id( ) ).c_str( ) );

                if ( nodeFb->child_ids( ) && nodeFb->child_ids( )->size( ) )
                    node.childIds.reserve( nodeFb->child_ids( )->size( ) );

                if ( nodeFb->material_ids( ) && nodeFb->material_ids( )->size( ) )
                    node.materialIds.reserve( nodeFb->material_ids( )->size( ) );

                auto childIdsIt = nodeFb->child_ids( )->data( );
                auto childIdsEndIt = childIdsIt + nodeFb->child_ids( )->size( );
                std::transform( childIdsIt, childIdsEndIt, std::back_inserter( node.childIds ), [&]( auto id ) {
                    auto &childNode    = scene->nodes[ id ];
                    childNode.parentId = node.id;
                    return id;
                } );

                if ( nodeFb->mesh_id( ) != -1 ) {
                    meshIds.insert( nodeFb->mesh_id( ) );
                }

                if ( nodeFb->material_ids( ) && nodeFb->material_ids( )->size( ) ) {
                    auto matIdsIt    = nodeFb->material_ids( )->data( );
                    auto matIdsEndIt = matIdsIt + nodeFb->material_ids( )->size( );
                    std::transform( matIdsIt, matIdsEndIt, std::back_inserter( node.materialIds ), [&]( auto id ) { return id; } );
                    materialIds.insert( matIdsIt, matIdsEndIt );
                }

                auto &transform   = scene->transforms[ nodeFb->id( ) ];
                auto  transformFb = ( *scene->sourceScene->transforms( ) )[ nodeFb->id( ) ];

                transform.translation.setX( transformFb->translation( ).x( ) );
                transform.translation.setY( transformFb->translation( ).y( ) );
                transform.translation.setZ( transformFb->translation( ).z( ) );
                transform.rotationOffset.setX( transformFb->rotation_offset( ).x( ) );
                transform.rotationOffset.setY( transformFb->rotation_offset( ).y( ) );
                transform.rotationOffset.setZ( transformFb->rotation_offset( ).z( ) );
                transform.rotationPivot.setX( transformFb->rotation_pivot( ).x( ) );
                transform.rotationPivot.setY( transformFb->rotation_pivot( ).y( ) );
                transform.rotationPivot.setZ( transformFb->rotation_pivot( ).z( ) );
                transform.preRotation.setX( transformFb->pre_rotation( ).x( ) * toRadsFactor );
                transform.preRotation.setY( transformFb->pre_rotation( ).y( ) * toRadsFactor );
                transform.preRotation.setZ( transformFb->pre_rotation( ).z( ) * toRadsFactor );
                transform.rotation.setX( transformFb->rotation( ).x( ) * toRadsFactor );
                transform.rotation.setY( transformFb->rotation( ).y( ) * toRadsFactor );
                transform.rotation.setZ( transformFb->rotation( ).z( ) * toRadsFactor );
                transform.postRotation.setX( transformFb->post_rotation( ).x( ) * toRadsFactor );
                transform.postRotation.setY( transformFb->post_rotation( ).y( ) * toRadsFactor );
                transform.postRotation.setZ( transformFb->post_rotation( ).z( ) * toRadsFactor );
                transform.scalingOffset.setX( transformFb->scaling_offset( ).x( ) );
                transform.scalingOffset.setY( transformFb->scaling_offset( ).y( ) );
                transform.scalingOffset.setZ( transformFb->scaling_offset( ).z( ) );
                transform.scalingPivot.setX( transformFb->scaling_pivot( ).x( ) );
                transform.scalingPivot.setY( transformFb->scaling_pivot( ).y( ) );
                transform.scalingPivot.setZ( transformFb->scaling_pivot( ).z( ) );
                transform.scaling.setX( transformFb->scaling( ).x( ) );
                transform.scaling.setY( transformFb->scaling( ).y( ) );
                transform.scaling.setZ( transformFb->scaling( ).z( ) );
                transform.geometricTranslation.setX( transformFb->geometric_translation( ).x( ) );
                transform.geometricTranslation.setY( transformFb->geometric_translation( ).y( ) );
                transform.geometricTranslation.setZ( transformFb->geometric_translation( ).z( ) );
                transform.geometricRotation.setX( transformFb->geometric_rotation( ).x( ) * toRadsFactor );
                transform.geometricRotation.setY( transformFb->geometric_rotation( ).y( ) * toRadsFactor );
                transform.geometricRotation.setZ( transformFb->geometric_rotation( ).z( ) * toRadsFactor );
                transform.geometricScaling.setX( transformFb->geometric_scaling( ).x( ) );
                transform.geometricScaling.setY( transformFb->geometric_scaling( ).y( ) );
                transform.geometricScaling.setZ( transformFb->geometric_scaling( ).z( ) );

                assert( transform.Validate( ) );
            }

            scene->UpdateMatrices( );
        }

        if ( auto meshesFb = scene->sourceScene->meshes( ) ) {
            // scene->meshes.reserve( meshesFb->size( ) );
            scene->meshes.reserve( meshIds.size( ) );

            for ( auto meshId : meshIds ) {

                // for ( auto meshFb : *meshesFb ) {
                auto meshFb = FlatbuffersTVectorGetAtIndex( meshesFb, meshId );

                assert( meshFb );
                assert( meshFb->vertices( ) && meshFb->vertices( )->size( ) );
                assert( meshFb->subsets( ) && meshFb->subsets( )->size( ) > 0 );
                assert( meshFb->submeshes( ) && meshFb->submeshes( )->size( ) == 1 );

                if ( meshFb->vertices( ) && meshFb->vertices( )->size( ) && meshFb->submeshes( ) && meshFb->submeshes( )->size( ) == 1 ) {

                    scene->meshes.emplace_back( );
                    auto &mesh = scene->meshes.back( );

                    auto submeshesFb = meshFb->submeshes( );
                    auto submeshFb = submeshesFb->Get( 0 );

                    mesh.positionOffset.setX( submeshFb->position_offset( ).x( ) );
                    mesh.positionOffset.setY( submeshFb->position_offset( ).y( ) );
                    mesh.positionOffset.setZ( submeshFb->position_offset( ).z( ) );
                    mesh.positionScale.setX( submeshFb->position_scale( ).x( ) );
                    mesh.positionScale.setY( submeshFb->position_scale( ).y( ) );
                    mesh.positionScale.setZ( submeshFb->position_scale( ).z( ) );
                    mesh.texcoordOffset.setX( submeshFb->uv_offset( ).x( ) );
                    mesh.texcoordOffset.setY( submeshFb->uv_offset( ).y( ) );
                    mesh.texcoordScale.setX( submeshFb->uv_scale( ).x( ) );
                    mesh.texcoordScale.setY( submeshFb->uv_scale( ).y( ) );

                    BufferLoadDesc vertexBufferLoadDesc = {};
                    memset( &vertexBufferLoadDesc, 0, sizeof( vertexBufferLoadDesc ) );

                    vertexBufferLoadDesc.mDesc.mUsage        = BUFFER_USAGE_VERTEX;
                    vertexBufferLoadDesc.mDesc.mMemoryUsage  = RESOURCE_MEMORY_USAGE_GPU_ONLY;
                    vertexBufferLoadDesc.mDesc.mSize         = submeshFb->vertex_count( ) * submeshFb->vertex_stride( );
                    vertexBufferLoadDesc.mDesc.mVertexStride = submeshFb->vertex_stride( );
                    vertexBufferLoadDesc.pData               = meshFb->vertices( ) + submeshFb->base_vertex( ) * submeshFb->vertex_stride( );
                    vertexBufferLoadDesc.ppBuffer            = &mesh.pVertexBuffer;

                    addResource( &vertexBufferLoadDesc );

                    if ( submeshFb->index_count( ) ) {

                        BufferLoadDesc indexBufferLoadDesc = {};
                        memset( &indexBufferLoadDesc, 0, sizeof( vertexBufferLoadDesc ) );

                        indexBufferLoadDesc.mDesc.mUsage       = BUFFER_USAGE_INDEX;
                        indexBufferLoadDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
                        indexBufferLoadDesc.mDesc.mSize        = submeshFb->index_count( ) * ( meshFb->index_type( ) + 2 );
                        indexBufferLoadDesc.mDesc.mIndexType   = meshFb->index_type( ) == apemodefb::EIndexTypeFb_UInt32 ? INDEX_TYPE_UINT32 : INDEX_TYPE_UINT16;
                        indexBufferLoadDesc.pData              = meshFb->indices( ) + submeshFb->base_index( ) * ( meshFb->index_type( ) + 2 );
                        indexBufferLoadDesc.ppBuffer           = &mesh.pIndexBuffer;

                        addResource( &indexBufferLoadDesc );
                    }

                    mesh.subsets.reserve( meshFb->subsets( )->size( ) );
                    std::transform( meshFb->subsets( )->begin( ),
                                    meshFb->subsets( )->end( ),
                                    std::back_inserter( mesh.subsets ),
                                    [&]( const apemodefb::SubsetFb *subsetFb ) {
                                        assert( subsetFb && subsetFb->index_count( ) );

                                        SceneMeshSubset subset;
                                        subset.materialId = subsetFb->material_id( );
                                        subset.baseIndex  = subsetFb->base_index( );
                                        subset.indexCount = subsetFb->index_count( );

                                        return subset;
                                    } );
                }
            }
        }


        if ( auto pMaterialsFb = scene->sourceScene->materials( ) ) {
            scene->materials.reserve( pMaterialsFb->size( ) );

            for ( auto materialId : materialIds ) {
                auto pMaterialFb = FlatbuffersTVectorGetAtIndex( pMaterialsFb, materialId );
                assert( pMaterialFb );

                LOGINFOFF( "Processing material {}", GetStringProperty( scene->sourceScene, pMaterialFb->name_id( ) ).c_str( ) );

                scene->materials.emplace_back( );
                auto &material = scene->materials.back( );

                if ( auto pPropertiesFb = pMaterialFb->properties( ) ) {
                    for ( auto pMaterialPropFb : *pPropertiesFb ) {

                        switch ( MaterialPropertyGetType( pMaterialPropFb->value_id( ) ) ) {
                            case apemodefb::EValueTypeFb::EValueTypeFb_String:
                                LOGINFOFF( "\t{} -> {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->value_id( ) ).c_str( ) );
                                break;

                            case apemodefb::EValueTypeFb::EValueTypeFb_Bool:
                                LOGINFOFF( "\t{} -> {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           GetBoolProperty( scene->sourceScene, pMaterialPropFb->value_id( ) ) );
                                break;

                            case apemodefb::EValueTypeFb::EValueTypeFb_Float:
                                LOGINFOFF( "\t{} -> {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           GetScalarProperty( scene->sourceScene, pMaterialPropFb->value_id( ) ));
                                break;

                            case apemodefb::EValueTypeFb::EValueTypeFb_Float2:
                                LOGINFOFF( "\t{} -> {} {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           (float) GetVec2Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getX( ),
                                           (float) GetVec2Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getY( ) );
                                break;

                            case apemodefb::EValueTypeFb::EValueTypeFb_Float3:
                                LOGINFOFF( "\t{} -> {} {} {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           (float) GetVec3Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getX( ),
                                           (float) GetVec3Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getY( ),
                                           (float) GetVec3Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getZ( ) );
                                break;

                            case apemodefb::EValueTypeFb::EValueTypeFb_Float4:
                                LOGINFOFF( "\t{} -> {} {} {} {}",
                                           GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                           (float) GetVec4Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getX( ),
                                           (float) GetVec4Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getY( ),
                                           (float) GetVec4Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getZ( ),
                                           (float) GetVec4Property( scene->sourceScene, pMaterialPropFb->value_id( ) ).getW( ) );
                                break;

                            default:
                                break;
                        }
                    }
                }

                TextureLoadDesc textureLoadDesc = {};
                if ( auto pTexturePropertiesFb = pMaterialFb->texture_properties( ) ) {
                    for ( auto pMaterialPropFb : *pTexturePropertiesFb ) {

                        LOGINFOFF( "\t{} -> {}",
                                   GetStringProperty( scene->sourceScene, pMaterialPropFb->name_id( ) ).c_str( ),
                                   pMaterialPropFb->value_id( ) );

                        if ( auto pTexturesFb = scene->sourceScene->textures( ) )
                            if ( auto pFilesFb = scene->sourceScene->files( ) ) {

                                if ( textureIdToTexture.find( pMaterialPropFb->value_id( ) ) == textureIdToTexture.end( ) ) {
                                    if ( auto pTextureFb = FlatbuffersTVectorGetAtIndex( pTexturesFb, pMaterialPropFb->value_id( ) ) ) {

                                        scene->images.emplace_back( conf_placement_new< Image >( conf_malloc( sizeof( Image ) ) ) );
                                        scene->textures.emplace_back( conf_placement_new< Texture >( conf_malloc( sizeof( Texture ) ) ) );

                                        auto pFileFb = FlatbuffersTVectorGetAtIndex( pFilesFb, pTextureFb->file_id( ) );
                                        auto fileName = FlatbuffersTVectorGetAtIndex( scene->sourceScene->string_values( ), MaterialPropertyGetIndex( pFileFb->name_id( ) ) );

                                        LOGINFOFF( "Loading texture: {}", GetStringProperty( scene->sourceScene, pTextureFb->name_id( ) ).c_str( ) );
                                        LOGINFOFF( "Loading texture from file: {}", fileName->c_str( ) );

                                        scene->images.back( )->iLoadSTBIFromMemory( (const char *) pFileFb->buffer( )->data( ), pFileFb->buffer( )->size( ), true );

                                        textureLoadDesc.pDesc       = nullptr;
                                        textureLoadDesc.mUseMipmaps = true;
                                        textureLoadDesc.pImage      = scene->images.back( );
                                        textureLoadDesc.ppTexture   = &scene->textures.back( );

                                        addResource( &textureLoadDesc );
                                        textureIdToTexture[ pMaterialPropFb->value_id( ) ] = scene->textures.back( );
                                    }
                                }
                            }
                    }
                }
            }
        }

        return std::move( scene );
    }

    return nullptr;
}
