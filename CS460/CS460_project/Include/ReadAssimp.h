#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

// Prints a mesh's info; A mesh contains vertices, faces, normals and
// more as needed for graphics.  Vertices are tied to bones with
// weights.
void showMesh(aiMesh* mesh);

// Prints an animation.  An animation contains a few timing parameters
// and then channels for a number of animated bones.  Each channel
// contains a V, Q, and S keyframe sequences.
void showAnimation(aiAnimation* anim);

// Prints the bone hierarchy and relevant info with a graphical
// representation of the hierarchy.
void showBoneHierarchy(const aiScene* scene, const aiNode* node, const int level = 0);

void ReadAssimpFile(const std::string& path);

unsigned GetKeyFramedBoneCount(const aiScene* scene);