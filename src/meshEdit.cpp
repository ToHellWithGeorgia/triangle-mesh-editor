#include <float.h>
#include <assert.h>
#include "meshEdit.h"
#include "mutablePriorityQueue.h"
#include "error_dialog.h"
#include <iostream>

namespace CS248 {

VertexIter HalfedgeMesh::splitEdge(EdgeIter e0) {
  // TODO: (meshEdit)
  // This method should split the given edge and return an iterator to the
  // newly inserted vertex. The halfedge of this vertex should point along
  // the edge that was split, rather than the new edges.
  // Gather the halfedges

  bool isBond = e0->isBoundary();
  HalfedgeIter h4 = e0->halfedge();
  if (h4->isBoundary()) {
    h4 = h4->twin();
  }
  HalfedgeIter h1 = h4->next();
  HalfedgeIter h0 = h1->twin();
  HalfedgeIter h2 = h1->next();
  HalfedgeIter h3 = h2->twin();
  HalfedgeIter h5 = h4->twin();
  HalfedgeIter h8;
  HalfedgeIter h9;
  HalfedgeIter h7;
  HalfedgeIter h6;
  if (!isBond) {
    h8 = h5->next();
    h9 = h8->twin();
    h7 = h8->next();
    h6 = h7->twin();
  }

  // Gather the vertices
  VertexIter v0 = h2->vertex();
  VertexIter v1 = h1->vertex();
  VertexIter v2 = h4->vertex();
  VertexIter v3;
  if (!isBond) {
    v3 = h9->vertex();
  }

  // Gather the edges
  EdgeIter e1 = h1->edge();
  EdgeIter e2 = h2->edge();
  EdgeIter e3;
  EdgeIter e4;
  if (!isBond) {
    e3 = h7->edge();
    e4 = h8->edge();
  }

  // Gather the faces
  FaceIter f0 = h1->face();
  FaceIter f1;
  if (!isBond){
    f1 = h7->face();
  }

  // Allocate new vertex.
  VertexIter nv = newVertex();
  nv->isNew = true;

  // Allocate new edges.
  int numNewEdge = isBond ? 3 : 4;
  std::vector<EdgeIter> ne;
  for (int i = 0; i < numNewEdge; i++){
    ne.push_back(newEdge());
    if (i != 1 && i != 2) {
      ne[i]->isNew = true;
    }
  }

  // Allocate new faces
  int numNewFace = isBond ? 2 : 4;
  std::vector<FaceIter> nf;
  for (int i = 0; i < numNewFace; i++){
    nf.push_back(newFace());
  }

  // Allocate new HalfEdges.
  int numNewHalfedge = isBond ? 6 : 8;
  std::vector<HalfedgeIter> nh;
  for (int i = 0; i < numNewHalfedge; i++){
    nh.push_back(newHalfedge());
  }


  // Reassign vertex
  Vector3D pos = e0->centroid();
  nv->position = pos;


  // Reassign half edges (next, twin, vertex, edge, face)
  h1->next() = nh[0];
  h1->face() = nf[0];
  h2->next() = nh[4];
  h2->face() = nf[1];
  if (!isBond){
    h7->next() = nh[3];
    h7->face() = nf[2];
    h8->next() = nh[7];
    h8->face() = nf[3];
  }
  nh[0]->setNeighbors(nh[2], nh[1], v0, ne[0], nf[0]);
  nh[1]->setNeighbors(h2, nh[0], nv, ne[0], nf[1]);
  nh[2]->setNeighbors(h1, nh[3], nv, ne[1], nf[0]);
  if (!isBond) {
    nh[3]->setNeighbors(nh[6], nh[2], v1, ne[1], nf[2]); 
    nh[5]->setNeighbors(h8, nh[4], nv, ne[2], nf[3]); 
  }
  else {
    nh[3]->setNeighbors(nh[5], nh[2], v1, ne[1], h5->face()); 
    // std::cout << h5->face()->isBoundary() << std::endl; 
    nh[5]->setNeighbors(h5->next(), nh[4], nv, ne[2], h5->face()); 
    h5->face()->halfedge() = nh[5];
  }
  nh[4]->setNeighbors(nh[1], nh[5], v2, ne[2], nf[1]);
  if (!isBond) {
    nh[6]->setNeighbors(h7, nh[7], nv, ne[3], nf[2]);
    nh[7]->setNeighbors(nh[5], nh[6], v3, ne[3], nf[3]);
  }
  // std::cout << "half dege finished" << std::endl;
  // Reassign edges
  ne[0]->halfedge() = nh[0];
  ne[1]->halfedge() = nh[2];
  ne[2]->halfedge() = nh[4];
  if (!isBond) {
    ne[3]->halfedge() = nh[6];
  }
  // std::cout << "dege finished" << std::endl;
  // Reassign faces
  nf[0]->halfedge() = h1;
  nf[1]->halfedge() = h2;
  if (!isBond) {
    nf[2]->halfedge() = h7;
    nf[3]->halfedge() = h8;
  }
  // std::cout << "face finished" << std::endl;
  // Reassign vertex
  v0->halfedge() = nh[0];
  v1->halfedge() = h1;
  v2->halfedge() = h3;
  if (!isBond) {
    v3->halfedge() = nh[7];
  }
  nv->halfedge() = nh[2];
  // std::cout << "vertex finished" << std::endl;
  if (isBond) {
    HalfedgeIter temp = nh[2];
    while (temp->next() != h5) {
      temp = temp->next()->twin();
    }
    temp->next() = nh[3];
  }

  // Delete unused halfedges, edges, faces
  deleteHalfedge(h4);
  deleteHalfedge(h5);
  deleteEdge(e0);
  deleteFace(f0);
  if (!isBond){
    deleteFace(f1);
  }

   
  // std::cout << "delete finished" << std::endl;
  return nv;
}

VertexIter HalfedgeMesh::collapseEdge(EdgeIter e) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // This method should collapse the given edge and return an iterator to
  // the new vertex created by the collapse.

  showError("collapseEdge() not implemented.");
  return VertexIter();
}

VertexIter HalfedgeMesh::collapseFace(FaceIter f) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // This method should collapse the given face and return an iterator to
  // the new vertex created by the collapse.
  showError("collapseFace() not implemented.");
  return VertexIter();
}

FaceIter HalfedgeMesh::eraseVertex(VertexIter v) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // This method should replace the given vertex and all its neighboring
  // edges and faces with a single face, returning the new face.
  
  // Cannot delete vertex on the boundary (or can we?)
  //
  if (v->isBoundary()) {
    return v->halfedge()->face();
  }

  HalfedgeIter hit = v->halfedge();
  // Start to gather all the delete edges, halfedges and faces
  std::vector<EdgeIter> all_delete_edges;
  std::vector<FaceIter> all_faces;
  std::vector<HalfedgeIter> all_delete_he;
  std::vector<HalfedgeIter> all_remain_he;

  auto new_face = newFace();

  do {
    hit = hit->twin();
    auto inner_hit = hit->next();
    // Delete the edges that are connected to the vertex
    all_delete_he.push_back(hit);
    all_delete_he.push_back(inner_hit);
    all_delete_edges.push_back(hit->edge());
    // all_delete_edges.push_back(inner_hit->edge());
    all_faces.push_back(hit->face());

    // Move the inner edge to the remain edges
    inner_hit = inner_hit->next();

    // Loop all the halfedges that will remain (not connected to vertex)
    do {
      all_remain_he.push_back(inner_hit);
      inner_hit = inner_hit->next();
    } while (inner_hit != hit);
    hit = hit->next();

  } while (hit != v->halfedge());

  // std::cout << "Gather elements complete!" << std::endl;
  // std::cout << "de_e: " << all_delete_edges.size()
  //           << " de_f: " << all_faces.size()
  //           << " de_he: " << all_delete_he.size()
  //           << " re_he: " << all_remain_he.size() << std::endl;

  // Delete all the stuff
  for (auto it = all_faces.begin(); it != all_faces.end(); it++) {
    deleteFace(*it);
  }

  // std::cout << "face" << std::endl;

  for (auto it = all_delete_edges.begin(); it != all_delete_edges.end(); it++) {
    deleteEdge(*it);
  }

  // std::cout << "edge" << std::endl;

  for (auto it = all_delete_he.begin(); it != all_delete_he.end(); it++) {
    deleteHalfedge(*it);
  }
  // std::cout << "he" << std::endl;

  deleteVertex(v);
  // std::cout << "Delete complete!" << std::endl;

  // Adjust the halfedges pointed by the vertex
  for (auto it = all_remain_he.begin(); it != all_remain_he.end(); it++) {
    (*it)->vertex()->halfedge() = *it;
  }

  // Adjust the connectivity of the new edge.
  hit = *(all_remain_he.begin());
  new_face->halfedge() = hit;
  do {
    hit->next() = hit->twin()->vertex()->halfedge();
    hit->face() = new_face;
    hit = hit->next();
  } while (hit != *all_remain_he.begin());
  
  // std::cout << "Adjust complete!" << std::endl;
  return new_face;
}

FaceIter HalfedgeMesh::eraseEdge(EdgeIter e) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // This method should erase the given edge and return an iterator to the
  // merged face.

  showError("eraseVertex() not implemented.");
  return FaceIter();
}

EdgeIter HalfedgeMesh::flipEdge(EdgeIter e0) {
  // TODO: (meshEdit)
  // This method should flip the given edge and return an iterator to the
  // flipped edge.

  // showError("flipEdge() not implemented.");

  // If the edge is on the boundary of the mesh, do nothing
  if (e0->isBoundary())
    return e0;

  // Gather the halfedges
  HalfedgeIter h4 = e0->halfedge();
  HalfedgeIter h1 = h4->next();
  HalfedgeIter h0 = h1->twin();
  HalfedgeIter h2 = h1->next();
  HalfedgeIter h3 = h2->twin();
  HalfedgeIter h5 = h4->twin();
  HalfedgeIter h8 = h5->next();
  HalfedgeIter h9 = h8->twin();
  HalfedgeIter h7 = h8->next();
  HalfedgeIter h6 = h7->twin();

  // Gather the vertices
  VertexIter v0 = h2->vertex();
  VertexIter v1 = h6->vertex();
  VertexIter v2 = h8->vertex();
  VertexIter v3 = h9->vertex();

  // Gather the edges
  EdgeIter e1 = h1->edge();
  EdgeIter e2 = h2->edge();
  EdgeIter e3 = h7->edge();
  EdgeIter e4 = h8->edge();

  // Gather the faces
  FaceIter f0 = h1->face();
  FaceIter f1 = h7->face();

  // Allocate new elements.
  h0->next() = h0->next();
  h0->twin() = h1;
  h0->vertex() = v0;
  h0->edge() = e1;
  h0->face() = h0->face();

  h1->next() = h4;
  h1->twin() = h0;
  h1->vertex() = v1;
  h1->edge() = e1;
  h1->face() = f0;

  h2->next() = h8;
  h2->twin() = h3;
  h2->vertex() = v0;
  h2->edge() = e2;
  h2->face() = f1;

  h3->next() = h3->next();
  h3->twin() = h2;
  h3->vertex() = v2;
  h3->edge() = e2;
  h3->face() = h3->face();

  h4->next() = h7;
  h4->twin() = h5;
  h4->vertex() = v0;
  h4->edge() = e0;
  h4->face() = f0;

  h5->next() = h2;
  h5->twin() = h4;
  h5->vertex() = v3;
  h5->edge() = e0;
  h5->face() = f1;

  h6->next() = h6->next();
  h6->twin() = h7;
  h6->vertex() = v1;
  h6->edge() = e3;
  h6->face() = h6->face();

  h7->next() = h1;
  h7->twin() = h6;
  h7->vertex() = v3;
  h7->edge() = e3;
  h7->face() = f0;

  h8->next() = h5;
  h8->twin() = h9;
  h8->vertex() = v2;
  h8->edge() = e4;
  h8->face() = f1;

  h9->next() = h9->next();
  h9->twin() = h8;
  h9->vertex() = v3;
  h9->edge() = e4;
  h9->face() = h9->face();

  v0->halfedge() = h0;
  v1->halfedge() = h6;
  v2->halfedge() = h3;
  v3->halfedge() = h9;

  e0->halfedge() = h4;
  e1->halfedge() = h1;
  e2->halfedge() = h2;
  e3->halfedge() = h7;
  e4->halfedge() = h8;

  f0->halfedge() = h1;
  f1->halfedge() = h2;


  return e0;
}

void HalfedgeMesh::subdivideQuad(bool useCatmullClark) {
  // Unlike the local mesh operations (like bevel or edge flip), we will perform
  // subdivision by splitting *all* faces into quads "simultaneously."  Rather
  // than operating directly on the halfedge data structure (which as you've
  // seen
  // is quite difficult to maintain!) we are going to do something a bit nicer:
  //
  //    1. Create a raw list of vertex positions and faces (rather than a full-
  //       blown halfedge mesh).
  //
  //    2. Build a new halfedge mesh from these lists, replacing the old one.
  //
  // Sometimes rebuilding a data structure from scratch is simpler (and even
  // more
  // efficient) than incrementally modifying the existing one.  These steps are
  // detailed below.

  // TODO Step I: Compute the vertex positions for the subdivided mesh.  Here
  // we're
  // going to do something a little bit strange: since we will have one vertex
  // in
  // the subdivided mesh for each vertex, edge, and face in the original mesh,
  // we
  // can nicely store the new vertex *positions* as attributes on vertices,
  // edges,
  // and faces of the original mesh.  These positions can then be conveniently
  // copied into the new, subdivided mesh.
  // [See subroutines for actual "TODO"s]
  if (useCatmullClark) {
    computeCatmullClarkPositions();
  } else {
    computeLinearSubdivisionPositions();
  }

  // TODO Step II: Assign a unique index (starting at 0) to each vertex, edge,
  // and
  // face in the original mesh.  These indices will be the indices of the
  // vertices
  // in the new (subdivided mesh).  They do not have to be assigned in any
  // particular
  // order, so long as no index is shared by more than one mesh element, and the
  // total number of indices is equal to V+E+F, i.e., the total number of
  // vertices
  // plus edges plus faces in the original mesh.  Basically we just need a
  // one-to-one
  // mapping between original mesh elements and subdivided mesh vertices.
  // [See subroutine for actual "TODO"s]
  assignSubdivisionIndices();

  // TODO Step III: Build a list of quads in the new (subdivided) mesh, as
  // tuples of
  // the element indices defined above.  In other words, each new quad should be
  // of
  // the form (i,j,k,l), where i,j,k and l are four of the indices stored on our
  // original mesh elements.  Note that it is essential to get the orientation
  // right
  // here: (i,j,k,l) is not the same as (l,k,j,i).  Indices of new faces should
  // circulate in the same direction as old faces (think about the right-hand
  // rule).
  // [See subroutines for actual "TODO"s]
  vector<vector<Index> > subDFaces;
  vector<Vector3D> subDVertices;
  buildSubdivisionFaceList(subDFaces);
  buildSubdivisionVertexList(subDVertices);

  // TODO Step IV: Pass the list of vertices and quads to a routine that clears
  // the
  // internal data for this halfedge mesh, and builds new halfedge data from
  // scratch,
  // using the two lists.
  rebuild(subDFaces, subDVertices);
}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * simple linear interpolation, e.g., the edge midpoints and face
 * centroids.
 */
void HalfedgeMesh::computeLinearSubdivisionPositions() {
  // TODO For each vertex, assign Vertex::newPosition to
  // its original position, Vertex::position.
  for (auto vit = verticesBegin(); vit != verticesEnd(); vit++) {
    vit->newPosition = vit->position;
  }

  // TODO For each edge, assign the midpoint of the two original
  // positions to Edge::newPosition.
  for (auto eit = edgesBegin(); eit != edgesEnd(); eit++) {
    // Gather the 2 vertices associated with the 2 halfedge
    eit->newPosition = eit->centroid();
  }

  // TODO For each face, assign the centroid (i.e., arithmetic mean)
  // of the original vertex positions to Face::newPosition.  Note
  // that in general, NOT all faces will be triangles!
  for (auto fit = facesBegin(); fit != facesEnd(); fit++) {
    fit->newPosition = fit->centroid();
  }

  // showError("computeLinearSubdivisionPositions() not implemented.");
}

/**
 * Compute new vertex positions for a mesh that splits each polygon
 * into quads (by inserting a vertex at the face midpoint and each
 * of the edge midpoints).  The new vertex positions will be stored
 * in the members Vertex::newPosition, Edge::newPosition, and
 * Face::newPosition.  The values of the positions are based on
 * the Catmull-Clark rules for subdivision.
 */
void HalfedgeMesh::computeCatmullClarkPositions() {
  // TODO The implementation for this routine should be
  // a lot like HalfedgeMesh::computeLinearSubdivisionPositions(),
  // except that the calculation of the positions themsevles is
  // slightly more involved, using the Catmull-Clark subdivision
  // rules. (These rules are outlined in the Developer Manual.)

  // TODO face
  for (auto fit = facesBegin(); fit != facesEnd(); fit++) {
    fit->newPosition = fit->centroid();
  }

  // TODO edges
  for (auto eit = edgesBegin(); eit != edgesEnd(); eit++) {
    if (!eit->isBoundary()){
      eit->newPosition = (eit->centroid() * 2 +
        eit->halfedge()->face()->newPosition +
        eit->halfedge()->twin()->face()->newPosition) / 4;
    }
    else {
      eit->newPosition = eit->centroid();
    }
  }

  // TODO vertices
  for (auto vit = verticesBegin(); vit != verticesEnd(); vit++) {
    HalfedgeIter he = vit->halfedge();
    Vector3D Q;
    Vector3D R;
    Vector3D RBond;
    do {
      Q += he->face()->newPosition;
      R += he->edge()->centroid();
      if (he->edge()->isBoundary()) {
        RBond += he->edge()->centroid();
      }
      he = he->twin()->next();
    } while (he != vit->halfedge());
    Size n = vit->degree();
    if (!vit->isBoundary()) {
      vit->newPosition = (Q / n + 2 * R / n + (n - 3) * vit->position) / n;
    }
    else {
      vit->newPosition = RBond / 8 + vit->position * 3 / 4;
    }
  }
  // showError("computeCatmullClarkPositions() not implemented.");
}

/**
 * Assign a unique integer index to each vertex, edge, and face in
 * the mesh, starting at 0 and incrementing by 1 for each element.
 * These indices will be used as the vertex indices for a mesh
 * subdivided using Catmull-Clark (or linear) subdivision.
 */
void HalfedgeMesh::assignSubdivisionIndices() {
  // TODO Start a counter at zero; if you like, you can use the
  // "Index" type (defined in halfedgeMesh.h)
  Index counter = 0;

  // TODO Iterate over vertices, assigning values to Vertex::index
  for (auto it = verticesBegin(); it != verticesEnd(); it++) {
    it->index = counter++;
  }

  // TODO Iterate over edges, assigning values to Edge::index
  for (auto it = edgesBegin(); it != edgesEnd(); it++) {
    it->index = counter++;
  }

  // TODO Iterate over faces, assigning values to Face::index
  for (auto it = facesBegin(); it != facesEnd(); it++) {
    it->index = counter++;
  }

  // showError("assignSubdivisionIndices() not implemented.");
}

/**
 * Build a flat list containing all the vertex positions for a
 * Catmull-Clark (or linear) subdivison of this mesh.  The order of
 * vertex positions in this list must be identical to the order
 * of indices assigned to Vertex::newPosition, Edge::newPosition,
 * and Face::newPosition.
 */
void HalfedgeMesh::buildSubdivisionVertexList(vector<Vector3D>& subDVertices) {
  // TODO Resize the vertex list so that it can hold all the vertices.

  // TODO Iterate over vertices, assigning Vertex::newPosition to the
  // appropriate location in the new vertex list.
  for (auto it = verticesBegin(); it != verticesEnd(); it++) {
    subDVertices.push_back(it->newPosition);
  }

  // TODO Iterate over edges, assigning Edge::newPosition to the appropriate
  // location in the new vertex list.
  for (auto it = edgesBegin(); it != edgesEnd(); it++) {
    subDVertices.push_back(it->newPosition);
  }

  // TODO Iterate over faces, assigning Face::newPosition to the appropriate
  // location in the new vertex list.
  for (auto it = facesBegin(); it != facesEnd(); it++) {
    subDVertices.push_back(it->newPosition);
  }

  // showError("buildSubdivisionVertexList() not implemented.");
}

/**
 * Build a flat list containing all the quads in a Catmull-Clark
 * (or linear) subdivision of this mesh.  Each quad is specified
 * by a vector of four indices (i,j,k,l), which come from the
 * members Vertex::index, Edge::index, and Face::index.  Note that
 * the ordering of these indices is important because it determines
 * the orientation of the new quads; it is also important to avoid
 * "bowties."  For instance, (l,k,j,i) has the opposite orientation
 * of (i,j,k,l), and if (i,j,k,l) is a proper quad, then (i,k,j,l)
 * will look like a bowtie.
 */
void HalfedgeMesh::buildSubdivisionFaceList(vector<vector<Index> >& subDFaces) {
  // TODO This routine is perhaps the most tricky step in the construction of
  // a subdivision mesh (second, perhaps, to computing the actual Catmull-Clark
  // vertex positions).  Basically what you want to do is iterate over faces,
  // then for each for each face, append N quads to the list (where N is the
  // degree of the face).  For this routine, it may be more convenient to simply
  // append quads to the end of the list (rather than allocating it ahead of
  // time), though YMMV.  You can of course iterate around a face by starting
  // with its first halfedge and following the "next" pointer until you get
  // back to the beginning.  The tricky part is making sure you grab the right
  // indices in the right order---remember that there are indices on vertices,
  // edges, AND faces of the original mesh.  All of these should get used.  Also
  // remember that you must have FOUR indices per face, since you are making a
  // QUAD mesh!

  // TODO iterate over faces
  // TODO loop around face
  // TODO build lists of four indices for each sub-quad
  // TODO append each list of four indices to face list
  for (auto it = facesBegin(); it != facesEnd(); it++) {
    HalfedgeIter he = it->halfedge();
    do {
      vector<Index> indexList(4);
      indexList[0] = he->edge()->index;
      indexList[1] = he->next()->vertex()->index;
      indexList[2] = he->next()->edge()->index;
      indexList[3] = it->index;
      he = he->next();
      subDFaces.push_back(indexList);
    } while (he != it->halfedge());
  }

  // showError("buildSubdivisionFaceList() not implemented.");
}

FaceIter HalfedgeMesh::bevelVertex(VertexIter v) {
  // *** Extra Credit ***
  // TODO This method should replace the vertex v with a face, corresponding to
  // a bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelVertexComputeNewPositions (which you also have to
  // implement!)

  // showError("bevelVertex() not implemented.");
  // return facesBegin();
  auto he_0 = v->halfedge();
  Size degree = v->degree();

  std::vector<HalfedgeIter> old_hes;
  std::vector<FaceIter> old_fcs;

  std::vector<HalfedgeIter> new_hes;
  std::vector<EdgeIter> new_edges;
  std::vector<VertexIter> new_vertices;

  // Gather the original halfedges and faces.
  //
  auto hit = he_0;
  do {
    hit = hit->twin();
    // Insert in the front so the order is aligned with
    // inner face (counter-clockwise)
    old_hes.insert(old_hes.begin(), hit->next());
    old_hes.insert(old_hes.begin(), hit);
    old_fcs.insert(old_fcs.begin(), hit->face());
    hit = hit->next();
  } while (hit != he_0);

  assert(old_fcs.size() == degree);

  // Allocate new faces, edges, vertices and halfedges
  // The order of these are also counter-clockwise
  FaceIter nf = newFace();
  for (int i = 0; i < degree; ++i) {
    // next twin vertex edge face
    HalfedgeIter nhe_0 = newHalfedge();
    HalfedgeIter nhe_1 = newHalfedge();
    EdgeIter ne = newEdge();
    VertexIter nv = newVertex();
    
    // Connectivity of the new vertex
    nv->halfedge() = nhe_0;
    // nv->position = old_hes[i * 2 + 1]->twin()->vertex()->position;
    // nv->position = v->position;
    nv->position = old_hes[i * 2 + 1]->edge()->centroid();
    ne->halfedge() = nhe_0;

    // Connectivity of the new halfedges
    // nhe_0 is inside the face, nhe_1 is outside
    // nhe_0->next() = ???
    nhe_0->twin() = nhe_1;
    nhe_0->vertex() = nv;
    nhe_0->edge() = ne;
    nhe_0->face() = nf;

    nhe_1->next() = old_hes[i * 2 + 1];
    nhe_1->twin() = nhe_0;
    // nhe_1->vertex() = next;
    nhe_1->edge() = ne;
    nhe_1->face() = old_fcs[i];

    // Fix the outer faces
    old_hes[i * 2]->next() = nhe_1;
    old_hes[i * 2 + 1]->vertex() = nv;

    new_hes.push_back(nhe_0);
    new_hes.push_back(nhe_1);
    new_edges.push_back(ne);
    new_vertices.push_back(nv);

    // Connect the missing part
    //
    if (i != 0) {
      new_hes[(i - 1) * 2]->next() = nhe_0;
      new_hes[(i - 1) * 2 + 1]->vertex() = nv;
    }
  }
  // Resolve the last missing piece
  (*(new_hes.end() - 2))->next() = new_hes[0];
  (*(new_hes.end() - 1))->vertex() = new_vertices[0];
  nf->halfedge() = new_hes[0];

  deleteVertex(v);
  return nf;
}

FaceIter HalfedgeMesh::bevelEdge(EdgeIter e) {
  // *** Extra Credit ***
  // TODO This method should replace the edge e with a face, corresponding to a
  // bevel operation. It should return the new face.  NOTE: This method is
  // responsible for updating the *connectivity* of the mesh only---it does not
  // need to update the vertex positions.  These positions will be updated in
  // HalfedgeMesh::bevelEdgeComputeNewPositions (which you also have to
  // implement!)

  showError("bevelEdge() not implemented.");
  return facesBegin();
}

FaceIter HalfedgeMesh::bevelFace(FaceIter f) {
  // *** Extra Credit ***
  // TODO This method should replace the face f with an additional, inset face
  // (and ring of faces around it), corresponding to a bevel operation. It
  // should return the new face.  NOTE: This method is responsible for updating
  // the *connectivity* of the mesh only---it does not need to update the vertex
  // positions.  These positions will be updated in
  // HalfedgeMesh::bevelFaceComputeNewPositions (which you also have to
  // implement!)

  showError("bevelFace() not implemented.");
  return facesBegin();
}


void HalfedgeMesh::bevelFaceComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double normalShift,
    double tangentialInset) {
  // *** Extra Credit ***
  // TODO Compute new vertex positions for the vertices of the beveled face.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the originalVertexPositions array) to compute an offset vertex
  // position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positions
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); hs++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

}

void HalfedgeMesh::bevelVertexComputeNewPositions(
    Vector3D originalVertexPosition, vector<HalfedgeIter>& newHalfedges,
    double tangentialInset) {
  // *** Extra Credit ***
  // TODO Compute new vertex positions for the vertices of the beveled vertex.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., hs.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.

  // We cap the largest phsical offset to be 0.0099.
  double scale = 1;
  double t = -1.0;
  // tangentialInset = abs(tangentialInset);
  double actualInset = tangentialInset * scale;
  for (auto he = newHalfedges.begin(); he != newHalfedges.end(); he++) {
    auto tmp = (*he)->twin();
    auto nv = (*he)->vertex();
    auto origNv = tmp->vertex();
    if (t < 0) {
      if (originalVertexPosition == nv->position) {
        t = 0;
      }
      else {
        t = (nv->position.x - originalVertexPosition.x) / (origNv->position.x - originalVertexPosition.x);
        if (std::isnan(t)) {
          t = (nv->position.y - originalVertexPosition.y) / (origNv->position.y - originalVertexPosition.y);
        }
        if (std::isnan(t)) {
          t = (nv->position.z - originalVertexPosition.z) / (origNv->position.z - originalVertexPosition.z);
        }
      }
    }
    t += actualInset;
    t = t >= 0.99 ? 0.99 : t;
    t = t <= 0.01 ? 0.01 : t;
    // std::cout<<t<<std::endl;
    nv->position = originalVertexPosition * (1 - t) + origNv->position * t;
    // std::cout<<nv->position<<std::endl;
  }
}

void HalfedgeMesh::bevelEdgeComputeNewPositions(
    vector<Vector3D>& originalVertexPositions,
    vector<HalfedgeIter>& newHalfedges, double tangentialInset) {
  // *** Extra Credit ***
  // TODO Compute new vertex positions for the vertices of the beveled edge.
  //
  // These vertices can be accessed via newHalfedges[i]->vertex()->position for
  // i = 1, ..., newHalfedges.size()-1.
  //
  // The basic strategy here is to loop over the list of outgoing halfedges,
  // and use the preceding and next vertex position from the original mesh
  // (in the orig array) to compute an offset vertex position.
  //
  // Note that there is a 1-to-1 correspondence between halfedges in
  // newHalfedges and vertex positions
  // in orig.  So, you can write loops of the form
  //
  // for( int i = 0; i < newHalfedges.size(); i++ )
  // {
  //    Vector3D pi = originalVertexPositions[i]; // get the original vertex
  //    position correponding to vertex i
  // }
  //

}

void HalfedgeMesh::splitPolygons(vector<FaceIter>& fcs) {
  for (auto f : fcs) splitPolygon(f);
}

void HalfedgeMesh::splitPolygon(FaceIter f) {
  // *** Extra Credit ***
  // TODO: (meshedit)
  // Triangulate a polygonal face

  // If the polygon is already a triangle, ignore.
  Size degree = f->degree();
  if (degree == 3) {
    return;
  }

  // Find the starting halfedge and vertex
  HalfedgeIter he_0 = f->halfedge();
  VertexIter v_0 = he_0->vertex();

  // Number of original edges that would compose a new triangle in the middle.
  int num_mid_edges = degree - 4;
  int num_new_edges = degree - 3;
  int num_new_he = num_new_edges * 2;
  int num_new_faces = degree - 2;

  std::vector<EdgeIter> new_edges;
  std::vector<HalfedgeIter> new_hes;
  std::vector<HalfedgeIter> mid_hes;
  std::vector<FaceIter> new_faces;

  // Create new faces
  for (int i = 0; i < num_new_faces; ++i) {
    // Don't delete the old face so the iteration can go on.
    if (i == 0) {
      new_faces.push_back(f);
      continue;
    }
    auto nf = newFace();
    new_faces.push_back(nf);
  }

  // Setup the new edges and halfedges
  for (int i = 0; i < num_new_edges; ++i) {
    auto ne = newEdge();
    auto nhe_0 = newHalfedge();
    auto nhe_1 = newHalfedge();
    new_edges.push_back(ne);
    new_hes.push_back(nhe_0);
    new_hes.push_back(nhe_1);
    ne->halfedge() = nhe_0;

    nhe_0->twin() = nhe_1;
    // nhe_0->next() = ???;
    // nhe_0->vertex() = ???;
    nhe_0->edge() = ne;
    nhe_0->face() = new_faces[i];

    nhe_1->twin() = nhe_0;
    // nhe_1->next() = ???;
    nhe_1->vertex() = v_0; // All new edges from the original points
    nhe_1->edge() = ne;
    nhe_1->face() = new_faces[i + 1];

    new_faces[i]->halfedge() = nhe_0;
    if (i == num_new_edges - 1) {
      new_faces[i+1]->halfedge() = nhe_1;
    }
  }

  // std::cout << "create complete." << std::endl;

  auto temphe = he_0->next()->next();
  // Gather the original middle edges
  for (int i = 0; i < num_mid_edges; ++i) {
    mid_hes.push_back(temphe);
    temphe = temphe->next();
  }

  // std::cout << "Gather complete." << std::endl;

  // Handle the first and last traingle separately. The others are the same.
  HalfedgeIter hit = he_0;
  hit->face() = new_faces[0];
  hit->next()->next() = new_hes[0];
  hit->next()->face() = new_faces[0];
  new_hes[0]->next() = hit;
  new_hes[0]->vertex() = hit->next()->twin()->vertex();

  (*(new_hes.end() - 1))->next() = temphe;
  temphe->face() = *(new_faces.end() - 1);
  temphe->next()->next() = (*(new_hes.end() - 1));
  temphe->next()->face() = *(new_faces.end() - 1);

  // std::cout << "Handle complete." << std::endl;

  // Resolve all the connectivity and vertex
  for (int i = 0; i < num_mid_edges; ++i) {
    auto mid_he = mid_hes[i];
    auto left_he = new_hes[2 * i + 1];
    auto right_he = new_hes[2 * i + 2];
    mid_he->next() = right_he;
    right_he->next() = left_he;
    left_he->next() = mid_he;
    right_he->vertex() = mid_he->twin()->vertex();
  }

  // std::cout << "Adjust complete." << std::endl;

  // showError("splitPolygon() not implemented.");
}

EdgeRecord::EdgeRecord(EdgeIter& _edge) : edge(_edge) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // Compute the combined quadric from the edge endpoints.
  // -> Build the 3x3 linear system whose solution minimizes the quadric error
  //    associated with these two endpoints.
  // -> Use this system to solve for the optimal position, and store it in
  //    EdgeRecord::optimalPoint.
  // -> Also store the cost associated with collapsing this edg in
  //    EdgeRecord::Cost.
}

void MeshResampler::upsample(HalfedgeMesh& mesh)
// This routine should increase the number of triangles in the mesh using Loop
// subdivision.
{
  // TODO: (meshEdit)
  // Compute new positions for all the vertices in the input mesh, using
  // the Loop subdivision rule, and store them in Vertex::newPosition.
  // -> At this point, we also want to mark each vertex as being a vertex of the
  //    original mesh.
  // -> Next, compute the updated vertex positions associated with edges, and
  //    store it in Edge::newPosition.
  // -> Next, we're going to split every edge in the mesh, in any order.  For
  //    future reference, we're also going to store some information about which
  //    subdivided edges come from splitting an edge in the original mesh, and
  //    which edges are new, by setting the flat Edge::isNew. Note that in this
  //    loop, we only want to iterate over edges of the original mesh.
  //    Otherwise, we'll end up splitting edges that we just split (and the
  //    loop will never end!)
  // -> Now flip any new edge that connects an old and new vertex.
  // -> Finally, copy the new vertex positions into final Vertex::position.

  // Each vertex and edge of the original surface can be associated with a
  // vertex in the new (subdivided) surface.
  // Therefore, our strategy for computing the subdivided vertex locations is to
  // *first* compute the new positions
  // using the connectity of the original (coarse) mesh; navigating this mesh
  // will be much easier than navigating
  // the new subdivided (fine) mesh, which has more elements to traverse.  We
  // will then assign vertex positions in
  // the new mesh based on the values we computed for the original mesh.

  // Compute updated positions for all the vertices in the original mesh, using
  // the Loop subdivision rule.
  for (auto it = mesh.verticesBegin(); it != mesh.verticesEnd(); it++) {
    it->isNew = false;
    Size ndeg = it->degree();
    double u = ndeg == 3 ? 3.0f / 16 : 3.0f / (8 * ndeg);
    auto he = it->halfedge();
    auto newPos = it->position * (1.0f - u * ndeg);
    Vector3D newPos_bond;
    do {
      he = he->twin();
      newPos += he->vertex()->position * u;
      newPos_bond += he->edge()->isBoundary() * he->edge()->centroid();
      he = he->next();
    } while (he != it->halfedge());
    newPos_bond = newPos_bond / 8 + it->position * 3 / 4;
    it->newPosition = it->isBoundary() ? newPos_bond : newPos;
  }

  // Next, compute the updated vertex positions associated with edges.
  for (auto it = mesh.edgesBegin(); it != mesh.edgesEnd(); it++) {
    it->isNew = false;
    auto he = it->halfedge();
    Vector3D newPos;
    if (it->isBoundary()) {
      newPos += 0.5 * he->vertex()->position + 0.5 *
                he->twin()->vertex()->position;
    } else {
      newPos += 0.375 * he->vertex()->position + 0.375 *
                he->twin()->vertex()->position;
      newPos += 0.125 * he->next()->twin()->vertex()->position;
      newPos += 0.125 * he->twin()->next()->twin()->vertex()->position;
    }
    it->newPosition = newPos;
  }
  // std::cout << "compute vertex finished" << std::endl;

  // Next, we're going to split every edge in the mesh, in any order.  For
  // future
  // reference, we're also going to store some information about which
  // subdivided
  // edges come from splitting an edge in the original mesh, and which edges are
  // new.
  // In this loop, we only want to iterate over edges of the original
  // mesh---otherwise,
  // we'll end up splitting edges that we just split (and the loop will never
  // end!)
  int ori_cnt = mesh.nEdges();
  auto it = mesh.edgesBegin();
  for (int cnt = 0; cnt < ori_cnt; ) {
    auto temp = it;
    temp++;
    auto newPos = it->newPosition;
    if (!it->isNew) {
      auto nv = mesh.splitEdge(it);
      nv->newPosition = newPos;
      cnt++;
    }
    it = temp;
  }
  // std::cout << "split finished" << std::endl;

  // Finally, flip any new edge that connects an old and new vertex.
  for (auto eit = mesh.edgesBegin(); eit != mesh.edgesEnd(); eit++) {
    if (eit->isNew) {
      if (eit->halfedge()->vertex()->isNew ^
          eit->halfedge()->twin()->vertex()->isNew) {
        mesh.flipEdge(eit);
      }
    }
  }

  // std::cout << "flip finished" << std::endl;
  // Copy the updated vertex positions to the subdivided mesh.
  // showError("upsample() not implemented.");
  for (auto vit = mesh.verticesBegin(); vit != mesh.verticesEnd(); vit++) {
    vit->position = vit->newPosition;
  }

  // std::cout << "copy finished" << std::endl;
}

void MeshResampler::downsample(HalfedgeMesh& mesh) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // Compute initial quadrics for each face by simply writing the plane equation
  // for the face in homogeneous coordinates. These quadrics should be stored
  // in Face::quadric
  // -> Compute an initial quadric for each vertex as the sum of the quadrics
  //    associated with the incident faces, storing it in Vertex::quadric
  // -> Build a priority queue of edges according to their quadric error cost,
  //    i.e., by building an EdgeRecord for each edge and sticking it in the
  //    queue.
  // -> Until we reach the target edge budget, collapse the best edge. Remember
  //    to remove from the queue any edge that touches the collapsing edge
  //    BEFORE it gets collapsed, and add back into the queue any edge touching
  //    the collapsed vertex AFTER it's been collapsed. Also remember to assign
  //    a quadric to the collapsed vertex, and to pop the collapsed edge off the
  //    top of the queue.
  showError("downsample() not implemented.");
}

void MeshResampler::resample(HalfedgeMesh& mesh) {
  // *** Extra Credit ***
  // TODO: (meshEdit)
  // Compute the mean edge length.
  // Repeat the four main steps for 5 or 6 iterations
  // -> Split edges much longer than the target length (being careful about
  //    how the loop is written!)
  // -> Collapse edges much shorter than the target length.  Here we need to
  //    be EXTRA careful about advancing the loop, because many edges may have
  //    been destroyed by a collapse (which ones?)
  // -> Now flip each edge if it improves vertex degree
  // -> Finally, apply some tangential smoothing to the vertex positions
  showError("resample() not implemented.");
}

}  // namespace CS248
