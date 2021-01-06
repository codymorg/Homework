#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const int MAX = 3; // Loops will print at most MAX entries followed by an elipsis.

// Prints a mesh's info; A mesh contains vertices, faces, normals and
// more as needed for graphics.  Vertices are tied to bones with
// weights.
void showMesh(aiMesh* mesh)
{
  // Mesh name and some counts
  printf("Mesh %s: %d vertices, %d faces,  %d bones\n", mesh->mName.C_Str(),
    mesh->mNumVertices, mesh->mNumFaces, mesh->mNumBones);

  // Mesh's bones and weights of all connected vertices.
  for (int i = 0; i < mesh->mNumBones && i < MAX; i++) {
    aiBone* bone = mesh->mBones[i];
    printf("  %s:  %d weights;  OffsetMatrix:[%f, ...]\n", bone->mName.C_Str(),
      bone->mNumWeights, bone->mOffsetMatrix[0][0]);
    for (int i = 0; i < bone->mNumWeights && i < MAX; i++) {
      printf("    %d %f\n", bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight);
    }
    if (bone->mNumWeights > MAX) printf("    ...\n");
  }
  if (mesh->mNumBones > MAX) printf("  ...\n");
}

// Prints an animation.  An animation contains a few timing parameters
// and then channels for a number of animated bones.  Each channel
// contains a V, Q, and S keyframe sequences.
void showAnimation(aiAnimation* anim)
{
  printf("Animation: %s\n  duration (in ticks): %f\n  tickspersecond: %f\n  numchannels: %d\n",
    anim->mName.C_Str(),
    anim->mDuration,
    anim->mTicksPerSecond,
    anim->mNumChannels);

  // The animations channels
  for (int i = 0; i < anim->mNumChannels && i < MAX; i++) {
    aiNodeAnim* chan = anim->mChannels[i];

    // Prints the bone name followed by the numbers of each key type
    printf("\n");
    printf("    %-15s VQS keys:  %d %d %d\n",
      chan->mNodeName.C_Str(),
      chan->mNumPositionKeys,
      chan->mNumRotationKeys,
      chan->mNumScalingKeys);

    // The position (V) keys
    printf("\n");
    for (int i = 0; i < chan->mNumPositionKeys && i < MAX; i++) {
      aiVectorKey key = chan->mPositionKeys[i];
      printf("      V[%d]: %f : (%f %f %f)\n", i, key.mTime,
        key.mValue[0], key.mValue[1], key.mValue[2]);
    }
    if (chan->mNumPositionKeys > MAX) printf("      ...\n");

    // The rotation (Q) keys
    printf("\n");
    for (int i = 0; i < chan->mNumRotationKeys && i < MAX; i++) {
      aiQuatKey key = chan->mRotationKeys[i];
      printf("      Q[%d]: %f : (%f %f %f %f)\n", i, key.mTime,
        key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
    }
    if (chan->mNumRotationKeys > MAX) printf("      ...\n");

    // the scaling (S) keys
    printf("\n");
    for (int i = 0; i < chan->mNumScalingKeys && i < MAX; i++) {
      aiVectorKey key = chan->mScalingKeys[i];
      printf("      S[%d]: %f : (%f %f %f)\n", i, key.mTime,
        key.mValue[0], key.mValue[1], key.mValue[2]);
    }
    if (chan->mNumScalingKeys > MAX) printf("      ...\n");
  }

  if (anim->mNumChannels > MAX) printf("    ...\n");
}

// Prints the bone hierarchy and relevant info with a graphical
// representation of the hierarchy.
void showBoneHierarchy(const aiScene* scene, const aiNode* node, const int level = 0)
{
  // Print indentation to show this node's level in the hierarchy
  for (int i = 0; i < level; i++)
    printf(" |");

  // Print node name and transformation to parent's node
  printf("%s                    Transformation:[%f, ...]\n", node->mName.C_Str(),
    node->mTransformation[0][0]);


  // Recurse onto this node's children
  for (unsigned int i = 0; i < node->mNumChildren; ++i)
    showBoneHierarchy(scene, node->mChildren[i], level + 1);
}

void ReadAssimpFile(const std::string& path)
{
  printf("Reading %s\n", path.c_str());
  Assimp::Importer importer;

  // A single call returning a single structure for the complete file.
  const aiScene* scene = importer.ReadFile(path.c_str(),
    aiProcess_Triangulate | aiProcess_GenNormals);

  printf("  %d animations\n", scene->mNumAnimations); // This is what 460/560 is all about
  printf("  %d meshes\n", scene->mNumMeshes);         // Verts and faces for the skin.
  printf("  %d materials\n", scene->mNumMaterials);   // More graphics info
  printf("  %d textures\n", scene->mNumTextures);     // More graphics info
  printf("\n");

  // Prints a graphical representation of the bone hierarchy.
  showBoneHierarchy(scene, scene->mRootNode);

  // Prints all the animation info for each animation in the file
  printf("\n");
  for (int i = 0; i < scene->mNumAnimations; i++)
    showAnimation(scene->mAnimations[i]);

  // Prints all the mesh info for each mesh in the file
  printf("\n");
  for (int i = 0; i < scene->mNumMeshes; i++)
    showMesh(scene->mMeshes[i]);
}

unsigned GetKeyFramedBoneCount(const aiScene* scene)
{
  std::map<std::string,bool> usedbones;

  for (int i = 0; i < scene->mNumAnimations; i++)
  {
    auto anim = scene->mAnimations[i];
    for (int i = 0; i < anim->mNumChannels && i < MAX; i++) 
    {
      aiNodeAnim* chan = anim->mChannels[i];
      usedbones.insert({chan->mNodeName.C_Str(), true});
    }
  }

  return usedbones.size();
}