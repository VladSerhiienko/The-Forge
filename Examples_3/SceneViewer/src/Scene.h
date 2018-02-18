
#include <map>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/util.h>

#include <FbxPipeline/generated/scene_generated.h>

#include <Common_3/OS/Interfaces/IMemoryManager.h>
#include <Common_3/OS/Math/MathTypes.h>
#include <Common_3/Renderer/IRenderer.h>
#include <Common_3/Renderer/ResourceLoader.h>

struct SceneMaterial {
    float    alphaCutoff;
    bool     bDoubleSided;
    vec3     emissiveFactor;
    vec4     baseColorFactor;
    float    metallicFactor;
    float    roughnessFactor;
    Texture *pBaseColorTexture;
    Texture *pNormalTexture;
    Texture *pOcclusionTexture;
    Texture *pEmissiveTexture;
    Texture *pMetallicRoughnessTexture;
};

struct SceneMeshSubset {
    uint32_t materialId = -1;
    uint32_t baseIndex  = 0;
    uint32_t indexCount = 0;
};

struct SceneMesh {
    Buffer *                       pVertexBuffer = nullptr;
    Buffer *                       pIndexBuffer  = nullptr;
    std::vector< SceneMeshSubset > subsets;
    vec3                           positionOffset;
    vec3                           positionScale;
    vec2                           texcoordOffset;
    vec2                           texcoordScale;
};

/**
 * Transfrom class that stores main FBX SDK transform properties
 * and calculates local and geometric matrices.
 **/
struct SceneNodeTransform {

    vec3 translation;
    vec3 rotationOffset;
    vec3 rotationPivot;
    vec3 preRotation;
    vec3 postRotation;
    vec3 rotation;
    vec3 scalingOffset;
    vec3 scalingPivot;
    vec3 scaling;
    vec3 geometricTranslation;
    vec3 geometricRotation;
    vec3 geometricScaling;

    /**
     * Checks for nans and zero scales.
     * @return True if valid, false otherwise.
     **/
    bool Validate( ) const;

    /**
     * Calculates local matrix.
     * @note Contributes node world matrix.
     * @return Node local matrix.
     **/
    mat4 CalculateLocalMatrix( ) const;

    /**
     * Calculate geometric transform.
     * @note That is an object-offset 3ds Max concept,
     *       it does not influence scene hierarchy, though
     *       it contributes to node world transform.
     * @return Node geometric transform.
     **/
    mat4 CalculateGeometricMatrix( ) const;
};

class Scene;
struct SceneNode {
    void *                      deviceAsset;
    Scene *                     scene    = nullptr;
    uint32_t                    id       = -1;
    uint32_t                    parentId = -1;
    uint32_t                    meshId   = -1;
    std::vector< uint32_t > childIds;
    std::vector< uint32_t > materialIds;
};

class Scene {
public:
    //
    // Scene components
    //

    std::string               sourceData;
    const apemodefb::SceneFb *sourceScene;

    std::vector< SceneNode >          nodes;
    std::vector< SceneNodeTransform > transforms;
    std::vector< SceneMesh >          meshes;
    std::vector< SceneMaterial >      materials;
    std::vector< Image * >            images;
    std::vector< Texture * >          textures;

    //
    // Transform matrices storage.
    //

    std::vector< mat4 > worldMatrices;
    std::vector< mat4 > localMatrices;
    std::vector< mat4 > geometricMatrices;
    std::vector< mat4 > hierarchicalMatrices;

    void *deviceAsset;

    /**
     * Internal usage only.
     * @see UpdateMatrices().
     * @todo Safety assertions.
     **/
    void UpdateChildWorldMatrices( uint32_t nodeId );

    /**
     * Update matrices storage with up-to-date values.
     * @todo Non-recursive, no dynamic memory?
     *       Currently, I tried to avoid any stack memory pollution,
     *       tried to avoid any local variable but kept it clean.
     **/
    void UpdateMatrices( );

    template < typename TNodeIdCallback >
    void ForEachNode( uint32_t nodeId, TNodeIdCallback callback ) {
        callback( nodeId );
        for ( auto &childId : nodes[ nodeId ].childIds ) {
            ForEachNode( childId, callback );
        }
    }

    template < typename TNodeIdCallback >
    void ForEachNode( TNodeIdCallback callback ) {
        ForEachNode( 0, callback );
    }
};

Scene *LoadSceneFromFile( const char *filename );
